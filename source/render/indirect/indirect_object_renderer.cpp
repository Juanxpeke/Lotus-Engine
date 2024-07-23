#include "indirect_object_renderer.h"

#include <algorithm>
#include "../../util/log.h"
#include "../../util/opengl_entry.h"
#include "../../util/opengl_extensions.h"
#include "../../util/profile.h"
#include "../../util/path_manager.h"
#include "../identifiers.h"

namespace Lotus {

  IndirectObjectRenderer::IndirectObjectRenderer() : vertexArrayID(0)
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

    std::shared_ptr<MeshObject> meshInstance = std::make_shared<MeshObject>(mesh, material);
    meshInstances.push_back(meshInstance);

    Handle<RenderMesh> meshHandle = getMeshHandle(mesh);
    Handle<RenderMaterial> materialHandle = getMaterialHandle(material);

    GPUObjectData GPUObject;
    GPUObject.model = meshInstance->getModelMatrix();
    GPUObject.materialHandle = materialHandle.get();
      
    uint32_t objectID = objectBuffer.add(&GPUObject);

    RenderObject renderObject;
    renderObject.model = GPUObject.model;
    renderObject.meshHandle = meshHandle;
    renderObject.materialHandle = materialHandle;
    renderObject.shaderHandle = static_cast<unsigned int>(material->getType());
    renderObject.ID = objectID;
    
    Handle<RenderObject> handle(static_cast<uint32_t>(renderObjects.size()));
    renderObjects.push_back(renderObject);
    
    uint32_t placeholderHandle = 1;
    objectHandleBuffer.add(&placeholderHandle);

    unbatchedObjectsHandles.push_back(handle);

    return meshInstance;
  }

  void IndirectObjectRenderer::render()
  {
    update();

    buildBatches();

    refreshBuffers();

    glBindVertexArray(vertexArrayID);
    
    indirectBuffer.bind();
    objectBuffer.bind();
    objectHandleBuffer.bind();
    materialBuffer.bind();

    for (int i = 0; i < shaderBatches.size(); i++)
    {
      const ShaderBatch& shaderBatch = shaderBatches[i];

      glUseProgram(shaders[shaderBatch.shaderHandle.get()].getProgramID());

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
  }

  void IndirectObjectRenderer::update()
  {
    updateObjects();
    updateMaterials();
  }

  void IndirectObjectRenderer::updateObjects()
  {
    for (int i = 0; i < meshInstances.size(); i++)
    {
      const std::shared_ptr<MeshObject>& meshInstance = meshInstances[i];
      Transform* transform = &(meshInstance->transform);

      if (transform->dirty || meshInstance->meshDirty || meshInstance->materialDirty)
      {
        RenderObject& renderObject = renderObjects[i];
        Handle<RenderObject> objectHandle(i);

        if (transform->dirty)
        {
          renderObject.model = meshInstance->getModelMatrix();
          transform->dirty = false;
        }
        if (meshInstance->materialDirty)
        {
          renderObject.materialHandle = getMaterialHandle(meshInstance->getMaterial());
          meshInstance->materialDirty = false;
        }
        if (meshInstance->meshDirty || meshInstance->shaderDirty)
        {
          if (!renderObject.unbatched)
          {
            /*
              The renderer MUST rearrange the batch related to this object if the mesh or the shader change
              so the batches ordering logic works accordingly.
            */
            toUnbatchObjects.push_back(renderObject);
            unbatchedObjectsHandles.push_back(objectHandle);

            renderObject.unbatched = true;
          }

          renderObject.meshHandle = getMeshHandle(meshInstance->getMesh());
          renderObject.shaderHandle = static_cast<unsigned int>(meshInstance->getMaterial()->getType());
          
          meshInstance->meshDirty = false;
          meshInstance->shaderDirty = false;
        }

        // Queue the object to be updated in the GPU buffer
        dirtyObjectsHandles.push_back(objectHandle);
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
        RenderMaterial& renderMaterial= renderMaterials[i];
        Handle<RenderMaterial> materialHandle(i);

        material->dirty = false;

        dirtyMaterialsHandles.push_back(materialHandle);
      }
    }
  }

  void IndirectObjectRenderer::buildBatches()
  {
    // Render merge
    buildObjectBatches();

    // Draw merge
    buildDrawBatches();

    // Shader merge
    buildShaderBatches();
  }

  void IndirectObjectRenderer::buildObjectBatches()
  {
    LOTUS_PROFILE_START_TIME(FrameTime::IndirectObjectBatchBuildTime);

    if (!toUnbatchObjects.empty())
    {

      std::vector<ObjectBatch> deletionObjectBatches;
      deletionObjectBatches.reserve(toUnbatchObjects.size());
      
      for (const RenderObject& object : toUnbatchObjects)
      {
        ObjectBatch batch;

        batch.objectHandle = Handle<RenderObject>(object.ID);
        batch.meshHandle = object.meshHandle;
        batch.shaderHandle = object.shaderHandle;

        deletionObjectBatches.push_back(batch);
      }

      toUnbatchObjects.clear();

      std::sort(deletionObjectBatches.begin(), deletionObjectBatches.end(),
      [](const ObjectBatch& bA, const ObjectBatch& bB) {
        if (bA.shaderHandle < bB.shaderHandle) { return true; }
        else if (bA.shaderHandle == bB.shaderHandle) {
                                if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
        }
        else { return false; }
      });

      std::vector<ObjectBatch> objectBatchesWithDeletion;
      objectBatchesWithDeletion.reserve(objectBatches.size());

      std::set_difference(objectBatches.begin(), objectBatches.end(), deletionObjectBatches.begin(), deletionObjectBatches.end(), std::back_inserter(objectBatchesWithDeletion),
      [](const ObjectBatch& bA, const ObjectBatch& bB) {
        if (bA.shaderHandle < bB.shaderHandle) { return true; }
        else if (bA.shaderHandle == bB.shaderHandle) {
                                if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
        }
        else { return false; }
      });

      objectBatches = std::move(objectBatchesWithDeletion);

    }

    if (!unbatchedObjectsHandles.empty())
    {
      std::vector<ObjectBatch> newObjectBatches;
      newObjectBatches.reserve(unbatchedObjectsHandles.size());
      
      // Fill new render batches
      for (auto objectHandle : unbatchedObjectsHandles)
      {
        RenderObject& object = renderObjects[objectHandle.get()];
        object.unbatched = false;

        ObjectBatch batch;
        batch.objectHandle = object.ID;		
        batch.meshHandle = object.meshHandle;
        batch.shaderHandle = object.shaderHandle;

        newObjectBatches.push_back(batch);
      }

      unbatchedObjectsHandles.clear();

      // New render batches sort
      std::sort(newObjectBatches.begin(), newObjectBatches.end(),
      [](const ObjectBatch& bA, const ObjectBatch& bB)
      {
        if (bA.shaderHandle < bB.shaderHandle) { return true; }
        else if (bA.shaderHandle == bB.shaderHandle) {
                      if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
        }
        else { return false; }
      });

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

        std::inplace_merge(begin, mid, end,
        [](const ObjectBatch& bA, const ObjectBatch& bB) {
          if (bA.shaderHandle < bB.shaderHandle) { return true; }
          else if (bA.shaderHandle == bB.shaderHandle) {
            if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
          }
          else { return false; }
        });
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
    LOTUS_PROFILE_START_TIME(FrameTime::IndirectDrawBatchBuildTime);

    drawBatches.clear();
    
    if (objectBatches.size() == 0) { return; }
    
    DrawBatch newDrawBatch;
    newDrawBatch.prevInstanceCount = 0;
    newDrawBatch.instanceCount = 0;
    newDrawBatch.meshHandle = objectBatches[0].meshHandle;
    newDrawBatch.shaderHandle = objectBatches[0].shaderHandle;

    drawBatches.push_back(newDrawBatch);
    DrawBatch* backDrawBatch = &drawBatches.back();

    for (int i = 0; i < objectBatches.size(); i++)
    {
      ObjectBatch* renderBatch = &objectBatches[i];

      bool bSameMesh = renderBatch->meshHandle == backDrawBatch->meshHandle;
      bool bSameShader = renderBatch->shaderHandle == backDrawBatch->shaderHandle;

      if (bSameMesh && bSameShader)
      {
        backDrawBatch->instanceCount++;
      }
      else
      {
        DrawBatch newDrawBatch;
        newDrawBatch.prevInstanceCount = i;
        newDrawBatch.instanceCount = 1;
        newDrawBatch.meshHandle = renderBatch->meshHandle;
        newDrawBatch.shaderHandle = renderBatch->shaderHandle;

        drawBatches.push_back(newDrawBatch);
        backDrawBatch = &drawBatches.back();
      }
    }

    LOTUS_PROFILE_END_TIME(FrameTime::IndirectDrawBatchBuildTime);
  }

  void IndirectObjectRenderer::buildShaderBatches()
  {
    LOTUS_PROFILE_START_TIME(FrameTime::IndirectShaderBatchBuildTime);

    shaderBatches.clear();

    if (drawBatches.size() == 0) { return; }

    ShaderBatch newShaderBatch;
    newShaderBatch.first = 0;
    newShaderBatch.count = 0;
    newShaderBatch.shaderHandle = drawBatches[0].shaderHandle;

    shaderBatches.push_back(newShaderBatch);
    ShaderBatch* backShaderBatch = &shaderBatches.back();

    for (int i = 0; i < drawBatches.size(); i++)
    {
      DrawBatch* drawBatch = &drawBatches[i];

      bool bSameShader = drawBatch->shaderHandle == backShaderBatch->shaderHandle;

      if (bSameShader)
      {
        backShaderBatch->count++;
      }
      else
      {
        ShaderBatch newShaderBatch;
        newShaderBatch.first = i;
        newShaderBatch.count = 1;
        newShaderBatch.shaderHandle = drawBatch->shaderHandle;

        shaderBatches.push_back(newShaderBatch);
        backShaderBatch = &shaderBatches.back();
      }
    }

    LOTUS_PROFILE_END_TIME(FrameTime::IndirectShaderBatchBuildTime);
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
    if (!drawBatches.empty())
    {
      indirectBuffer.resize(drawBatches.size());

      DrawElementsIndirectCommand* indirectBufferMap = indirectBuffer.map();

      for (int i = 0; i < drawBatches.size(); i++)
      {
        auto drawBatch = drawBatches[i];

        const RenderMesh& mesh = renderMeshes[drawBatch.meshHandle.get()];

        indirectBufferMap[i].count = mesh.count;
        indirectBufferMap[i].instanceCount = drawBatch.instanceCount;
        indirectBufferMap[i].firstIndex = mesh.firstIndex;
        indirectBufferMap[i].baseVertex = mesh.baseVertex;
        indirectBufferMap[i].baseInstance = drawBatch.prevInstanceCount;
      }

      indirectBuffer.unmap();
    }
  }

  void IndirectObjectRenderer::refreshObjectBuffer()
  {
    if (dirtyObjectsHandles.empty())
    {
      return;
    }

    GPUObjectData* objectBufferMap = objectBuffer.map();
    
    for (const Handle<RenderObject>& objectHandle : dirtyObjectsHandles)
    {
      const RenderObject& object = renderObjects[objectHandle.get()];

      objectBufferMap[object.ID].model = object.model;
      objectBufferMap[object.ID].materialHandle = object.materialHandle.get();
    }

    objectBuffer.unmap();

    dirtyObjectsHandles.clear();
  }

  void IndirectObjectRenderer::refreshObjectHandleBuffer()
  {
    if (drawBatches.empty())
    {
      return;
    }

    uint32_t* objectHandleBufferMap = objectHandleBuffer.map();

    int index = 0;
    for (int dI = 0; dI < drawBatches.size(); dI++)
    {
      auto drawBatch = drawBatches[dI];

      for (int iI = 0; iI < drawBatch.instanceCount; iI++)
      {
        objectHandleBufferMap[index] = renderObjects[objectBatches[drawBatch.prevInstanceCount + iI].objectHandle.get()].ID;
        index++;
      }
    }

    objectHandleBuffer.unmap();
  }
  
  void IndirectObjectRenderer::refreshMaterialBuffer()
  {
    if (dirtyMaterialsHandles.empty())
    {
      return;
    }

    GPUMaterialData* materialBufferMap = materialBuffer.map();

    for (const Handle<RenderMaterial>& materialHandle : dirtyMaterialsHandles)
    {
      const RenderMaterial& renderMaterial = renderMaterials[materialHandle.get()];

      const std::shared_ptr<Material>& material = materials[materialHandle.get()];

      materialBufferMap[renderMaterial.ID] = materials[materialHandle.get()]->getMaterialData();
    }

    materialBuffer.unmap();

    dirtyMaterialsHandles.clear();
  }

  Handle<RenderMesh> IndirectObjectRenderer::getMeshHandle(std::shared_ptr<Mesh> mesh)
  {
    Handle<RenderMesh> handle;

    auto it = meshMap.find(mesh);

    if (it == meshMap.end())
    {
      const std::vector<MeshVertex>& vertices = mesh->getVertices();
      const std::vector<unsigned int>& indices = mesh->getIndices();

      uint32_t verticesBufferLocation = vertexBuffer.add(vertices.data(), vertices.size()); 
      uint32_t indicesBufferLocation = indexBuffer.add(indices.data(), indices.size());

      RenderMesh renderMesh;
      renderMesh.firstIndex = indicesBufferLocation;
      renderMesh.baseVertex = verticesBufferLocation;
      renderMesh.count = indices.size();

      handle.set(static_cast<uint32_t>(renderMeshes.size()));
      renderMeshes.push_back(renderMesh);

      meshMap[mesh] = handle;
    }
    else
    {
      handle = (*it).second;
    }

    return handle;
  }

  Handle<RenderMaterial> IndirectObjectRenderer::getMaterialHandle(std::shared_ptr<Material> material)
  {
    Handle<RenderMaterial> handle;

    auto it = materialMap.find(material);

    if (it == materialMap.end())
    {
      materials.push_back(material);

      GPUMaterialData GPUMaterial = material->getMaterialData();
      
      uint32_t materialID = materialBuffer.add(&GPUMaterial);

      RenderMaterial renderMaterial;
      renderMaterial.ID = materialID;
      
      handle.set(static_cast<uint32_t>(renderMaterials.size()));
      renderMaterials.push_back(renderMaterial);
      
      materialMap[material] = handle;
    }
    else
    {
      handle = (*it).second;
    }

    return handle;
  }

}