#include "indirect_scene.h"

#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../util/path_manager.h"

namespace Lotus {

  IndirectScene::IndirectScene() : vertexArrayID(0)
  {
    shaders[static_cast<unsigned int>(MaterialType::UnlitFlat)] = ShaderProgram(shaderPath("indirect/unlit_flat.vert"), shaderPath("indirect/unlit_flat.frag"));
    shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("indirect/diffuse_flat.vert"), shaderPath("indirect/diffuse_flat.frag"));
    
    glGenVertexArrays(1, &vertexArrayID);

    GPUVertexBuffer.allocate(VertexBufferInitialAllocationSize);
    GPUVertexBuffer.setVertexArray(vertexArrayID);

    GPUIndexBuffer.allocate(IndexBufferInitialAllocationSize);
    GPUIndexBuffer.setVertexArray(vertexArrayID);

    GPUIndirectBuffer.allocate(IndirectBufferInitialAllocationSize);

    GPUObjectBuffer.allocate(ObjectBufferInitialAllocationSize);
    GPUObjectBuffer.setBindingPoint(ObjectBufferBindingPoint);

    GPUObjectHandleBuffer.allocate(ObjectBufferInitialAllocationSize);
    GPUObjectHandleBuffer.setBindingPoint(ObjectHandleBufferBindingPoint);

    GPUMaterialBuffer.allocate(MaterialBufferInitialAllocationSize);
    GPUMaterialBuffer.setBindingPoint(MaterialBufferBindingPoint);
  }

  IndirectScene::~IndirectScene()
  {
    if (vertexArrayID)
    {
      glDeleteVertexArrays(1, &vertexArrayID);

      vertexArrayID = 0;
    }
  }

  std::shared_ptr<MeshInstance> IndirectScene::createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
  {
    std::shared_ptr<MeshInstance> meshInstance = std::make_shared<MeshInstance>(mesh, material);
    meshInstances.push_back(meshInstance);

    Handle<RenderMesh> meshHandle = getMeshHandle(mesh);
    Handle<RenderMaterial> materialHandle = getMaterialHandle(material);

    GPUObjectData GPUObject;
    GPUObject.model = meshInstance->getModelMatrix();
    GPUObject.materialHandle = materialHandle.get();
      
    uint32_t objectID = GPUObjectBuffer.add(&GPUObject);

    RenderObject renderObject;
    renderObject.model = GPUObject.model;
    renderObject.meshHandle = meshHandle;
    renderObject.materialHandle = materialHandle;
    renderObject.shaderHandle = static_cast<unsigned int>(material->getType());
    renderObject.ID = objectID;
    
    Handle<RenderObject> handle(static_cast<uint32_t>(renderObjects.size()));
    renderObjects.push_back(renderObject);
    
    uint32_t placeholderHandle = 1;
    GPUObjectHandleBuffer.add(&placeholderHandle);

    unbatchedObjectsHandles.push_back(handle);

    return meshInstance;
  }

  void IndirectScene::deleteObject(std::shared_ptr<MeshInstance> meshInstance)
  {
    meshInstances[0] = std::move(meshInstances.back());
    meshInstances.pop_back();
  }

  std::shared_ptr<Material> IndirectScene::createMaterial(MaterialType type)
  {
    unsigned int offset = static_cast<unsigned int>(type);

    switch (type)
    {
    case MaterialType::UnlitFlat:
      return std::make_shared<UnlitFlatMaterial>();
      break;
    case MaterialType::DiffuseFlat:
      return std::make_shared<DiffuseFlatMaterial>();
      break;
    case MaterialType::DiffuseTextured:
      return std::make_shared<DiffuseFlatMaterial>();
      break;
    case MaterialType::MaterialTypeCount:
      return std::make_shared<DiffuseFlatMaterial>();
      break;
    default:
      return nullptr;
      break;
    }
  }

  void IndirectScene::render(const Camera& camera)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::vec3 cameraPosition = camera.getLocalTranslation();
   
    update();

    buildBatches();

    refreshBuffers();

    glBindVertexArray(vertexArrayID);
    
    GPUIndirectBuffer.bind();
    GPUObjectBuffer.bind();
    GPUObjectHandleBuffer.bind();
    GPUMaterialBuffer.bind();

    for (int i = 0; i < shaderBatches.size(); i++)
    {
      const ShaderBatch& shaderBatch = shaderBatches[i];

      glUseProgram(shaders[shaderBatch.shaderHandle.get()].getProgramID());
      
      glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
      glUniformMatrix4fv(ProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

      glMultiDrawElementsIndirect(
          GL_TRIANGLES,
          GL_UNSIGNED_INT,
          (void*) (shaderBatch.first * sizeof(DrawElementsIndirectCommand)),
          shaderBatch.count,
          sizeof(DrawElementsIndirectCommand));
    }

    GPUMaterialBuffer.unbind();
    GPUObjectBuffer.unbind();
    GPUObjectHandleBuffer.unbind();
    GPUIndirectBuffer.unbind();
  }

  void IndirectScene::update()
  {
    updateObjects();
    updateMaterials();
  }

  void IndirectScene::updateObjects()
  {
    for (int i = 0; i < meshInstances.size(); i++)
    {
      const std::shared_ptr<MeshInstance>& meshInstance = meshInstances[i];
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

  void IndirectScene::updateMaterials()
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

  void IndirectScene::buildBatches()
  {
    // Render merge
    buildObjectBatches();

    // Draw merge
    buildDrawBatches();

    // Shader merge
    buildShaderBatches();
  }

  void IndirectScene::buildObjectBatches()
  {
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
  }

  void IndirectScene::buildDrawBatches()
  {
    drawBatches.clear();
    GPUIndirectBuffer.filledSize = 0;
    
    if (objectBatches.size() == 0) { return; }
    
    DrawBatch newDrawBatch;
    newDrawBatch.prevInstanceCount = 0;
    newDrawBatch.instanceCount = 0;
    newDrawBatch.meshHandle = objectBatches[0].meshHandle;
    newDrawBatch.shaderHandle = objectBatches[0].shaderHandle;

    drawBatches.push_back(newDrawBatch);
    DrawBatch* backDrawBatch = &drawBatches.back();

    GPUIndirectBuffer.filledSize++;

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

        GPUIndirectBuffer.filledSize++;
      }
    }

  }

  void IndirectScene::buildShaderBatches()
  {
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

  }

  void IndirectScene::refreshBuffers()
  {
    refreshIndirectBuffer();
    refreshObjectBuffer();
    refreshObjectHandleBuffer();
    refreshMaterialBuffer();
  }

  void IndirectScene::refreshIndirectBuffer()
  {
    if (!drawBatches.empty())
    {
      DrawElementsIndirectCommand* indirectBuffer = GPUIndirectBuffer.map();

      for (int i = 0; i < drawBatches.size(); i++)
      {
        auto drawBatch = drawBatches[i];

        const RenderMesh& mesh = renderMeshes[drawBatch.meshHandle.get()];

        indirectBuffer[i].count = mesh.count;
        indirectBuffer[i].instanceCount = drawBatch.instanceCount;
        indirectBuffer[i].firstIndex = mesh.firstIndex;
        indirectBuffer[i].baseVertex = mesh.baseVertex;
        indirectBuffer[i].baseInstance = drawBatch.prevInstanceCount;
      }

      GPUIndirectBuffer.unmap();
    }
  }

  void IndirectScene::refreshObjectBuffer()
  {
    if (dirtyObjectsHandles.empty())
    {
      return;
    }

    GPUObjectData* objectBuffer = GPUObjectBuffer.map();
    
    for (const Handle<RenderObject>& objectHandle : dirtyObjectsHandles)
    {
      const RenderObject& object = renderObjects[objectHandle.get()];

      objectBuffer[object.ID].model = object.model;
      objectBuffer[object.ID].materialHandle = object.materialHandle.get();
    }

    GPUObjectBuffer.unmap();

    dirtyObjectsHandles.clear();
  }

  void IndirectScene::refreshObjectHandleBuffer()
  {
    if (drawBatches.empty())
    {
      return;
    }

    uint32_t* objectHandleBuffer = GPUObjectHandleBuffer.map();

    int index = 0;
    for (int dI = 0; dI < drawBatches.size(); dI++)
    {
      auto drawBatch = drawBatches[dI];

      for (int iI = 0; iI < drawBatch.instanceCount; iI++)
      {
        objectHandleBuffer[index] = renderObjects[objectBatches[drawBatch.prevInstanceCount + iI].objectHandle.get()].ID;
        index++;
      }
    }

    GPUObjectHandleBuffer.unmap();
  }
  
  void IndirectScene::refreshMaterialBuffer()
  {
    if (dirtyMaterialsHandles.empty())
    {
      return;
    }

    GPUMaterialData* materialBuffer = GPUMaterialBuffer.map();

    for (const Handle<RenderMaterial>& materialHandle : dirtyMaterialsHandles)
    {
      const RenderMaterial& renderMaterial = renderMaterials[materialHandle.get()];

      const std::shared_ptr<Material>& material = materials[materialHandle.get()];

      materialBuffer[renderMaterial.ID] = materials[materialHandle.get()]->getMaterialData();
    }

    GPUMaterialBuffer.unmap();

    dirtyMaterialsHandles.clear();
  }

  void IndirectScene::refreshInstancesBuffer()
  {
		if (0 < objectBatches.size())
		{
			// reallocateBuffer(CPU_GPUInstanceBuffer, renderBatches.size() * sizeof(GPUInstance));
		}

    if (objectBatches.size() > 0)
    {
      GPUInstance* data = nullptr; // TODO

      int dataIndex = 0;
      for (int dI = 0; dI < drawBatches.size(); dI++)
      {
        auto drawBatch = drawBatches[dI];

        for (int iI = 0; iI < drawBatch.instanceCount; iI++)
        {
          data[dataIndex].objectID = 0; //(&objects[renderBatches[drawBatch.prevInstancesCount + iI].objectHandle.get()])->objectHandle.get();
          data[dataIndex].drawBatchID = dI;
          dataIndex++;
        }
      }
    }
  }

  Handle<RenderMesh> IndirectScene::getMeshHandle(std::shared_ptr<Mesh> mesh)
  {
    Handle<RenderMesh> handle;

    auto it = meshMap.find(mesh);

    if (it == meshMap.end())
    {
      const std::vector<Vertex>& vertices = mesh->getVertices();
      const std::vector<unsigned int>& indices = mesh->getIndices();

      uint32_t verticesBufferLocation = GPUVertexBuffer.add(vertices.data(), vertices.size()); 
      uint32_t indicesBufferLocation = GPUIndexBuffer.add(indices.data(), indices.size());

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

  Handle<RenderMaterial> IndirectScene::getMaterialHandle(std::shared_ptr<Material> material)
  {
    Handle<RenderMaterial> handle;

    auto it = materialMap.find(material);

    if (it == materialMap.end())
    {
      materials.push_back(material);

      GPUMaterialData GPUMaterial = material->getMaterialData();
      
      uint32_t materialID = GPUMaterialBuffer.add(&GPUMaterial);

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