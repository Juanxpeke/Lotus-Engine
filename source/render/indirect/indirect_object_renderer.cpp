#include "indirect_object_renderer.h"

#include <algorithm>
#include "../../util/log.h"
#include "../../util/opengl_entry.h"
#include "../../util/opengl_extensions.h"
#include "../../util/profile.h"
#include "../../util/path_manager.h"
#include "../identifiers.h"

namespace Lotus {

  IndirectObjectRenderer::IndirectObjectRenderer() : vertexArrayID(0), objectBatchesModified(false)
  {
    supportsTexturedMaterials = OpenGLExtensionChecker::isExtensionSupported(OpenGLExtension::BindlessTexture);

    shaders[static_cast<unsigned int>(MaterialType::UnlitFlat)] = ShaderProgram(shaderPath("indirect/unlit_flat.vert"), shaderPath("indirect/unlit_flat.frag"));
    shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("indirect/diffuse_flat.vert"), shaderPath("indirect/diffuse_flat.frag"));

    if (supportsTexturedMaterials)
    {
      shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(shaderPath("indirect/diffuse_textured.vert"), shaderPath("indirect/diffuse_textured.frag"));
    }

    glGenVertexArrays(1, &vertexArrayID);

    vertexBuffer.allocate(VertexBufferInitialAllocationSize);
    vertexBuffer.setVertexArray(vertexArrayID);

    indexBuffer.allocate(IndexBufferInitialAllocationSize);
    indexBuffer.setVertexArray(vertexArrayID);

    indirectBuffer.allocate(IndirectBufferInitialAllocationSize);

    objectBuffer.allocate(ObjectBufferInitialAllocationSize);
    objectBuffer.setBindingPoint(ObjectBufferBindingPoint);

    objectHandleBuffer.allocate(ObjectBufferInitialAllocationSize);
    objectHandleBuffer.setBindingPoint(ObjectHandleBufferBindingPoint);

    materialBuffer.allocate(MaterialBufferInitialAllocationSize);
    materialBuffer.setBindingPoint(MaterialBufferBindingPoint);
  }

  IndirectObjectRenderer::~IndirectObjectRenderer()
  {
    if (vertexArrayID)
    {
      glDeleteVertexArrays(1, &vertexArrayID);

      vertexArrayID = 0;
    }
  }

  std::shared_ptr<MeshObject> IndirectObjectRenderer::createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
  {
    LOTUS_PROFILE_INCREASE_COUNTER(FrameCounter::AddedIndirectObjects);

    std::shared_ptr<MeshObject> object = std::make_shared<MeshObject>(mesh, material);
    objects.push_back(object);

    Handler<IndirectRenderMesh> meshHandler = getMeshHandler(mesh);
    Handler<IndirectRenderMaterial> materialHandler = getMaterialHandler(material);

    glm::mat4 model = object->getModelMatrix();

    GPUObjectData GPUObject;
    GPUObject.model = model;
    GPUObject.materialHandle = materialHandler.handle;
      
    uint32_t objectID = objectBuffer.add(&GPUObject);

    IndirectRenderObject renderObject;
    renderObject.model = model;
    renderObject.mesh = meshHandler;
    renderObject.material = materialHandler;
    renderObject.shader.handle = static_cast<uint32_t>(material->getType());
    renderObject.ID = objectID;
    
    Handler<IndirectRenderObject> handler(static_cast<uint32_t>(renderObjects.size()));
    renderObjects.push_back(renderObject);

    unbatchedObjectsHandlers.push_back(handler);

    return object;
  }

  void IndirectObjectRenderer::render()
  {
    update();

    buildBatches();

    refreshBuffers();

    LOTUS_PROFILE_START_TIME(FrameTime::IndirectSceneRenderTime);

    glBindVertexArray(vertexArrayID);
    
    indirectBuffer.bind();
    objectBuffer.bind();
    objectHandleBuffer.bind();
    materialBuffer.bind();

    for (int i = 0; i < shaderBatches.size(); i++)
    {
      const ShaderBatch& shaderBatch = shaderBatches[i];

      glUseProgram(shaders[shaderBatch.shader.handle].getProgramID());

      glMultiDrawElementsIndirect(
          GL_TRIANGLES,
          GL_UNSIGNED_INT,
          (void*) (shaderBatch.first * sizeof(DrawElementsIndirectCommand)),
          shaderBatch.count,
          sizeof(DrawElementsIndirectCommand));
    }

    materialBuffer.unbind();
    objectBuffer.unbind();
    objectHandleBuffer.unbind();
    indirectBuffer.unbind();

    glBindVertexArray(0);

    LOTUS_PROFILE_END_TIME(FrameTime::IndirectSceneRenderTime);
  }

  void IndirectObjectRenderer::update()
  {
    updateObjects();
    updateMaterials();
  }

  void IndirectObjectRenderer::updateObjects()
  {
    for (int i = 0; i < objects.size(); i++)
    {
      const std::shared_ptr<MeshObject>& object = objects[i];
      Transform* transform = &(object->transform);

      if (transform->dirty || object->meshDirty || object->materialDirty)
      {
        IndirectRenderObject& renderObject = renderObjects[i];
        Handler<IndirectRenderObject> objectHandle(i);

        if (transform->dirty)
        {
          renderObject.model = object->getModelMatrix();
          transform->dirty = false;
        }
        if (object->materialDirty)
        {
          renderObject.material = getMaterialHandler(object->getMaterial());
          object->materialDirty = false;
        }
        if (object->meshDirty || object->shaderDirty)
        {
          if (!renderObject.unbatched)
          {
            /*
              The renderer MUST rearrange the batch related to this object if the mesh or the shader change
              so the batches ordering logic works accordingly.
            */
            toUnbatchObjects.push_back(renderObject);
            unbatchedObjectsHandlers.push_back(objectHandle);

            renderObject.unbatched = true;
          }

          renderObject.mesh = getMeshHandler(object->getMesh());
          renderObject.shader.handle = static_cast<uint32_t>(object->getMaterial()->getType());
          
          object->meshDirty = false;
          object->shaderDirty = false;
        }

        // Queue the object to be updated in the GPU buffer
        dirtyObjectsHandlers.push_back(objectHandle);
      }
    }
  }

  void IndirectObjectRenderer::updateMaterials()
  {
    for (int i = 0; i < materials.size(); i++)
    {
      const std::shared_ptr<Material>& material = materials[i];

      if (material->dirty)
      {
        IndirectRenderMaterial& renderMaterial= renderMaterials[i];
        Handler<IndirectRenderMaterial> materialHandle(i);

        material->dirty = false;

        dirtyMaterialsHandlers.push_back(materialHandle);
      }
    }
  }

  void IndirectObjectRenderer::buildBatches()
  {
    buildObjectBatches();
    buildDrawBatches();
    buildShaderBatches();
  }

  void IndirectObjectRenderer::buildObjectBatches()
  {
    LOTUS_PROFILE_START_TIME(FrameTime::IndirectObjectBatchBuildTime);

    objectBatchesModified = !(toUnbatchObjects.empty() && unbatchedObjectsHandlers.empty());

    if (!toUnbatchObjects.empty())
    {
      std::vector<ObjectBatch> deletionObjectBatches;
      deletionObjectBatches.reserve(toUnbatchObjects.size());
      
      for (const IndirectRenderObject& object : toUnbatchObjects)
      {
        ObjectBatch batch;

        batch.object = Handler<IndirectRenderObject>(object.ID);
        batch.mesh = object.mesh;
        batch.shader = object.shader;

        deletionObjectBatches.push_back(batch);
      }

      toUnbatchObjects.clear();

      std::sort(deletionObjectBatches.begin(), deletionObjectBatches.end());

      std::vector<ObjectBatch> objectBatchesWithDeletion;
      objectBatchesWithDeletion.reserve(objectBatches.size());

      std::set_difference(objectBatches.begin(), objectBatches.end(), deletionObjectBatches.begin(), deletionObjectBatches.end(), std::back_inserter(objectBatchesWithDeletion));

      objectBatches = std::move(objectBatchesWithDeletion);
    }

    if (!unbatchedObjectsHandlers.empty())
    {
      std::vector<ObjectBatch> newObjectBatches;
      newObjectBatches.reserve(unbatchedObjectsHandlers.size());
      
      // Fill new render batches
      for (auto objectHandler : unbatchedObjectsHandlers)
      {
        IndirectRenderObject& object = renderObjects[objectHandler.handle];
        object.unbatched = false;

        ObjectBatch batch;
        batch.object.handle = object.ID;		
        batch.mesh = object.mesh;
        batch.shader = object.shader;

        newObjectBatches.push_back(batch);
      }

      unbatchedObjectsHandlers.clear();

      // New render batches sort
      std::sort(newObjectBatches.begin(), newObjectBatches.end());

      // Merge the new render batches into the main render batch array
      if (!objectBatches.empty() && !newObjectBatches.empty())
      {
        int index = objectBatches.size();
        objectBatches.reserve(objectBatches.size() + newObjectBatches.size());
        
        for (const ObjectBatch& objectBatch : newObjectBatches)
        {
          objectBatches.push_back(objectBatch);
        }

        ObjectBatch* begin = objectBatches.data();
        ObjectBatch* mid = begin + index;
        ObjectBatch* end = begin + objectBatches.size();

        std::inplace_merge(begin, mid, end);
      }
      else if (objectBatches.empty())
      {
        objectBatches = std::move(newObjectBatches);
      }

    }

    LOTUS_PROFILE_END_TIME(FrameTime::IndirectObjectBatchBuildTime);
  }

  void IndirectObjectRenderer::buildDrawBatches()
  {
    if (objectBatchesModified)
    {
      drawBatches.clear();
      
      if (!objectBatches.empty())
      {
        LOTUS_PROFILE_START_TIME(FrameTime::IndirectDrawBatchBuildTime);
        
        DrawBatch newDrawBatch;
        newDrawBatch.prevInstanceCount = 0;
        newDrawBatch.instanceCount = 0;
        newDrawBatch.mesh = objectBatches[0].mesh;
        newDrawBatch.shader = objectBatches[0].shader;

        drawBatches.push_back(newDrawBatch);
        DrawBatch* backDrawBatch = &drawBatches.back();

        for (int i = 0; i < objectBatches.size(); i++)
        {
          ObjectBatch* renderBatch = &objectBatches[i];

          bool bSameMesh = renderBatch->mesh.handle == backDrawBatch->mesh.handle;
          bool bSameShader = renderBatch->shader.handle == backDrawBatch->shader.handle;

          if (bSameMesh && bSameShader)
          {
            backDrawBatch->instanceCount++;
          }
          else
          {
            DrawBatch newDrawBatch;
            newDrawBatch.prevInstanceCount = i;
            newDrawBatch.instanceCount = 1;
            newDrawBatch.mesh = renderBatch->mesh;
            newDrawBatch.shader = renderBatch->shader;

            drawBatches.push_back(newDrawBatch);
            backDrawBatch = &drawBatches.back();
          }
        }

        LOTUS_PROFILE_END_TIME(FrameTime::IndirectDrawBatchBuildTime);
      }
    }
  }

  void IndirectObjectRenderer::buildShaderBatches()
  {
    if (objectBatchesModified)
    {
      shaderBatches.clear();

      if (!drawBatches.empty())
      {
        LOTUS_PROFILE_START_TIME(FrameTime::IndirectShaderBatchBuildTime);

        ShaderBatch newShaderBatch;
        newShaderBatch.first = 0;
        newShaderBatch.count = 0;
        newShaderBatch.shader = drawBatches[0].shader;

        shaderBatches.push_back(newShaderBatch);
        ShaderBatch* backShaderBatch = &shaderBatches.back();

        for (int i = 0; i < drawBatches.size(); i++)
        {
          DrawBatch* drawBatch = &drawBatches[i];

          bool bSameShader = drawBatch->shader.handle == backShaderBatch->shader.handle;

          if (bSameShader)
          {
            backShaderBatch->count++;
          }
          else
          {
            ShaderBatch newShaderBatch;
            newShaderBatch.first = i;
            newShaderBatch.count = 1;
            newShaderBatch.shader = drawBatch->shader;

            shaderBatches.push_back(newShaderBatch);
            backShaderBatch = &shaderBatches.back();
          }
        }

        LOTUS_PROFILE_END_TIME(FrameTime::IndirectShaderBatchBuildTime);
      }
    }
  }

  void IndirectObjectRenderer::refreshBuffers()
  {
    refreshIndirectBuffer();
    refreshObjectBuffer();
    refreshObjectHandleBuffer();
    refreshMaterialBuffer();
  }

  void IndirectObjectRenderer::refreshIndirectBuffer()
  {
    if (objectBatchesModified && !drawBatches.empty())
    {
      LOTUS_PROFILE_START_TIME(Lotus::FrameTime::IndirectIndirectBufferRefreshTime);

      indirectBuffer.resize(drawBatches.size());

      DrawElementsIndirectCommand* indirectBufferMap = indirectBuffer.map();

      for (int i = 0; i < drawBatches.size(); i++)
      {
        const DrawBatch& drawBatch = drawBatches[i];
        const IndirectRenderMesh& mesh = renderMeshes[drawBatch.mesh.handle];

        indirectBufferMap[i].count = mesh.count;
        indirectBufferMap[i].instanceCount = drawBatch.instanceCount;
        indirectBufferMap[i].firstIndex = mesh.firstIndex;
        indirectBufferMap[i].baseVertex = mesh.baseVertex;
        indirectBufferMap[i].baseInstance = drawBatch.prevInstanceCount;
      }

      indirectBuffer.unmap();
      
      LOTUS_PROFILE_END_TIME(Lotus::FrameTime::IndirectIndirectBufferRefreshTime);
    }
  }

  void IndirectObjectRenderer::refreshObjectBuffer()
  {
    if (!dirtyObjectsHandlers.empty())
    {
      LOTUS_PROFILE_START_TIME(Lotus::FrameTime::IndirectObjectBufferRefreshTime);

      GPUObjectData* objectBufferMap = objectBuffer.map();
      
      for (const Handler<IndirectRenderObject>& objectHandler : dirtyObjectsHandlers)
      {
        const IndirectRenderObject& object = renderObjects[objectHandler.handle];

        objectBufferMap[object.ID].model = object.model;
        objectBufferMap[object.ID].materialHandle = object.material.handle;
      }

      objectBuffer.unmap();

      dirtyObjectsHandlers.clear();

      LOTUS_PROFILE_END_TIME(Lotus::FrameTime::IndirectObjectBufferRefreshTime);
    }
  }

  void IndirectObjectRenderer::refreshObjectHandleBuffer()
  {
    if (objectBatchesModified && !drawBatches.empty())
    {
      LOTUS_PROFILE_START_TIME(Lotus::FrameTime::IndirectObjectHandleBufferRefreshTime);

      objectHandleBuffer.resize(objectBatches.size());

      uint32_t* objectHandleBufferMap = objectHandleBuffer.map();

      int index = 0;
      for (int dI = 0; dI < drawBatches.size(); dI++)
      {
        auto drawBatch = drawBatches[dI];

        for (int iI = 0; iI < drawBatch.instanceCount; iI++)
        {
          objectHandleBufferMap[index] = renderObjects[objectBatches[drawBatch.prevInstanceCount + iI].object.handle].ID;
          index++;
        }
      }

      objectHandleBuffer.unmap();

      LOTUS_PROFILE_END_TIME(Lotus::FrameTime::IndirectObjectHandleBufferRefreshTime);
    }
  }
  
  void IndirectObjectRenderer::refreshMaterialBuffer()
  {
    if (!dirtyMaterialsHandlers.empty())
    {
      LOTUS_PROFILE_START_TIME(Lotus::FrameTime::IndirectMaterialBufferRefreshTime);

      GPUMaterialData* materialBufferMap = materialBuffer.map();

      for (const Handler<IndirectRenderMaterial>& materialHandler : dirtyMaterialsHandlers)
      {
        const IndirectRenderMaterial& renderMaterial = renderMaterials[materialHandler.handle];

        const std::shared_ptr<Material>& material = materials[materialHandler.handle];

        materialBufferMap[renderMaterial.ID] = materials[materialHandler.handle]->getMaterialData();
      }

      materialBuffer.unmap();

      dirtyMaterialsHandlers.clear();

      LOTUS_PROFILE_END_TIME(Lotus::FrameTime::IndirectMaterialBufferRefreshTime);
    }
  }

  Handler<IndirectRenderMesh> IndirectObjectRenderer::getMeshHandler(const std::shared_ptr<Mesh>& mesh)
  {
    Handler<IndirectRenderMesh> handler;

    auto it = meshMap.find(mesh);

    if (it == meshMap.end())
    {
      const std::vector<MeshVertex>& vertices = mesh->getVertices();
      const std::vector<unsigned int>& indices = mesh->getIndices();

      uint32_t verticesBufferLocation = vertexBuffer.add(vertices.data(), vertices.size()); 
      uint32_t indicesBufferLocation = indexBuffer.add(indices.data(), indices.size());

      IndirectRenderMesh renderMesh;
      renderMesh.firstIndex = indicesBufferLocation;
      renderMesh.baseVertex = verticesBufferLocation;
      renderMesh.count = indices.size();

      handler.handle = static_cast<uint32_t>(renderMeshes.size());
      renderMeshes.push_back(renderMesh);

      meshMap[mesh] = handler;
    }
    else
    {
      handler = (*it).second;
    }

    return handler;
  }

  Handler<IndirectRenderMaterial> IndirectObjectRenderer::getMaterialHandler(const std::shared_ptr<Material>& material)
  {
    Handler<IndirectRenderMaterial> handler;

    auto it = materialMap.find(material);

    if (it == materialMap.end())
    {
      materials.push_back(material);

      GPUMaterialData GPUMaterial = material->getMaterialData();
      
      uint32_t materialID = materialBuffer.add(&GPUMaterial);

      IndirectRenderMaterial renderMaterial;
      renderMaterial.ID = materialID;
      
      handler.handle = static_cast<uint32_t>(renderMaterials.size());
      renderMaterials.push_back(renderMaterial);
      
      materialMap[material] = handler;
    }
    else
    {
      handler = (*it).second;
    }

    return handler;
  }

}