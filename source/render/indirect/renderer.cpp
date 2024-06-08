#include "renderer.h"

#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../util/path_manager.h"

namespace Lotus {

  void printRenderBatches(const std::vector<ObjectBatch>& batches)
  {
    for (int i = 0; i < batches.size(); i++)
    {
      std::cout << "RB[" << i << "] = {oh_" << batches[i].objectHandle.get() << "mh_" << batches[i].meshHandle.get() << "sh_" << batches[i].shaderHandle.get() << "}" << std::endl; 
    }
    std::cout << std::endl;
  }

  Renderer::Renderer() :
    vertexArrayID(0),
    ambientLight({1.0, 1.0, 1.0})
  {}

  void Renderer::startUp()
  {
    shaders[static_cast<unsigned int>(0)] = ShaderProgram(shaderPath("n_diffuse_flat.vert"), shaderPath("n_diffuse_flat.frag"));
    //shaders[static_cast<unsigned int>(1)] = ShaderProgram(shaderPath("n_unlit_flat.vert"), shaderPath("n_unlit_flat.frag"));
    terrainShader = ShaderProgram(shaderPath("terrain.vert"), shaderPath("terrain.frag"));

    glEnable(GL_DEPTH_TEST);
    
    glGenVertexArrays(1, &vertexArrayID);

    glGenBuffers(1, &lightBufferID);

    GPUVertexBuffer.allocate(VertexBufferInitialAllocationSize);
    GPUVertexBuffer.setVertexArray(vertexArrayID);

    GPUIndexBuffer.allocate(IndexBufferInitialAllocationSize);
    GPUIndexBuffer.setVertexArray(vertexArrayID);

    GPUIndirectBuffer.allocate(IndirectBufferInitialAllocationSize);

    glBindBuffer(GL_UNIFORM_BUFFER, lightBufferID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPULightsData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, LightUBOBindingPoint, lightBufferID);

    GPUObjectBuffer.allocate(ObjectBufferInitialAllocationSize);
    GPUObjectBuffer.setBindingPoint(ObjectBufferBindingPoint);

    GPUObjectHandleBuffer.allocate(ObjectBufferInitialAllocationSize);
    GPUObjectHandleBuffer.setBindingPoint(ObjectHandleBufferBindingPoint);

    GPUMaterialBuffer.allocate(MaterialBufferInitialAllocationSize);
    GPUMaterialBuffer.setBindingPoint(MaterialBufferBindingPoint);

    glfwSwapInterval(0);
  }

  Renderer::~Renderer()
  {
    if (vertexArrayID)
    {
      glDeleteBuffers(1, &lightBufferID);
      glDeleteVertexArrays(1, &vertexArrayID);

      vertexArrayID = 0;
    }
  }

  std::shared_ptr<MeshInstance> Renderer::createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
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
    renderObject.meshHandle = getMeshHandle(mesh);
    renderObject.materialHandle = getMaterialHandle(material);
    renderObject.shaderID = 0;
    renderObject.ID = objectID;
    
    Handle<RenderObject> handle(static_cast<uint32_t>(renderObjects.size()));
    renderObjects.push_back(renderObject);
    
    uint32_t xd = 1;
    GPUObjectHandleBuffer.add(&xd);

    unbatchedObjectsHandles.push_back(handle);

    return meshInstance;
  }

  void Renderer::deleteMeshInstance(std::shared_ptr<MeshInstance> meshInstance)
  {
    meshInstances[0] = std::move(meshInstances.back());
    meshInstances.pop_back();
  }

  std::shared_ptr<Material> Renderer::createMaterial(MaterialType type)
  {
    unsigned int offset = static_cast<unsigned int>(type);

    switch (type)
    {
    case MaterialType::DiffuseFlat:
      return std::make_shared<DiffuseFlatMaterial>(shaders[0]);
      break;
    case MaterialType::DiffuseTextured:
      return std::make_shared<DiffuseFlatMaterial>(shaders[0]);
      break;
    case MaterialType::MaterialTypeCount:
      return std::make_shared<DiffuseFlatMaterial>(shaders[0]);
      break;
    default:
      return nullptr;
      break;
    }
  }

  std::shared_ptr<Terrain> Renderer::createTerrain()
  {
    terrain = std::make_shared<Terrain>(64);
    
    return terrain;
  }

  void Renderer::setAmbientLight(glm::vec3 color)
  {
    ambientLight = color;
  }

  std::shared_ptr<DirectionalLight> Renderer::createDirectionalLight()
  {
    std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
    directionalLights.push_back(directionalLight);

    return directionalLight;
  }

  std::shared_ptr<PointLight> Renderer::createPointLight()
  {
    std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
    pointLights.push_back(pointLight);

    return pointLight;
  }

  void Renderer::render(const Camera& camera)
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

      glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*) shaderBatch.first, shaderBatch.count, sizeof(DrawElementsIndirectCommand));
    }

    GPUMaterialBuffer.unbind();
    GPUObjectBuffer.unbind();
    GPUObjectHandleBuffer.unbind();
    GPUIndirectBuffer.unbind();

    if (terrain != nullptr)
    {
      glUseProgram(terrainShader.getProgramID());

      glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
      glUniformMatrix4fv(ProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

      glBindVertexArray(terrain->vertexArrayID);

      glDrawElements(GL_TRIANGLES, terrain->indexBuffer.filledSize, GL_UNSIGNED_INT, nullptr);

      glBindVertexArray(0);
    }
  }

  void Renderer::update()
  {
    updateObjects();
    updateMaterials();
  }

  void Renderer::updateObjects()
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
          renderObject.shaderID = 0;// meshInstance->getMaterial()->getShaderID();
          
          meshInstance->meshDirty = false;
          meshInstance->shaderDirty = false;
        }

        // Queue the object to be updated in the GPU buffer
        dirtyObjectsHandles.push_back(objectHandle);
      }
    }
  }

  void Renderer::updateMaterials()
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

  void Renderer::buildBatches()
  {
    // Render merge
    buildObjectBatches();

    // Draw merge
    buildDrawBatches();

    // Shader merge
    buildShaderBatches();
  }

  void Renderer::buildObjectBatches()
  {
    if (!toUnbatchObjects.empty())
    {
      std::cout << "Render batches" << std::endl;
      printRenderBatches(objectBatches);

      std::vector<ObjectBatch> deletionObjectBatches;
      deletionObjectBatches.reserve(toUnbatchObjects.size());
      
      for (const RenderObject& object : toUnbatchObjects)
      {
        ObjectBatch batch;

        batch.objectHandle = Handle<RenderObject>(object.ID);
        batch.meshHandle = object.meshHandle;
        batch.shaderHandle = object.shaderID;

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

      // std::cout << "Ordered deletion render batches" << std::endl;
      // printRenderBatches(deletionObjectBatches);

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

      // std::cout << "Render batches after deletion" << std::endl;
      // printRenderBatches(objectBatches);
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
        batch.shaderHandle = object.shaderID;

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

      //std::cout << "Ordered new render batches" << std::endl;
      //printRenderBatches(newObjectBatches);

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

      // std::cout << "Render batches after addition" << std::endl;
      // printRenderBatches(objectBatches);
    }
  }

  void Renderer::buildDrawBatches()
  {
    drawBatches.clear();
    GPUIndirectBuffer.filledSize = 0;
    
    if (objectBatches.size() == 0) { return; }
    
    DrawBatch newDrawBatch;
    newDrawBatch.prevInstanceCount = 0;
    newDrawBatch.instanceCount = 0;
    newDrawBatch.meshHandle = objectBatches[0].meshHandle;

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

        drawBatches.push_back(newDrawBatch);
        backDrawBatch = &drawBatches.back();

        GPUIndirectBuffer.filledSize++;
      }
    }
  }

  void Renderer::buildShaderBatches()
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

  void Renderer::refreshBuffers()
  {
    refreshIndirectBuffer();
    refreshLightBuffer();
    refreshObjectBuffer();
    refreshObjectHandleBuffer();
    refreshMaterialBuffer();
  }

  void Renderer::refreshIndirectBuffer()
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

  void Renderer::refreshLightBuffer()
  {
    GPULightsData lightsData;

    lightsData.ambientLight = ambientLight;

    uint32_t directionalLightsCount = std::min(static_cast<uint32_t>(HalfMaxDirectionalLights * 2), static_cast<uint32_t>(directionalLights.size()));
    lightsData.directionalLightsCount = static_cast<int>(directionalLightsCount);

    for (uint32_t i = 0; i < directionalLightsCount; i++)
    {
      const std::shared_ptr<DirectionalLight>& dirLight = directionalLights[i];
      lightsData.directionalLights[i].colorIntensity = dirLight->getLightColor() * dirLight->getLightIntensity();
      lightsData.directionalLights[i].direction = glm::rotate(dirLight->getLightDirection(), dirLight->getFrontVector());
    }

    uint32_t pointLightsCount = std::min(static_cast<uint32_t>(HalfMaxPointLights * 2), static_cast<uint32_t>(pointLights.size()));
    lightsData.pointLightsCount = static_cast<int>(pointLightsCount);

    for (uint32_t i = 0; i < pointLightsCount; i++)
    {
      const std::shared_ptr<PointLight>& pointLight = pointLights[i];
      lightsData.pointLights[i].colorIntensity = pointLight->getLightColor() * pointLight->getLightIntensity();
      lightsData.pointLights[i].position = pointLight->getLocalTranslation();
      lightsData.pointLights[i].radius = pointLight->getLightRadius();
    }

    glBindBuffer(GL_UNIFORM_BUFFER, lightBufferID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPULightsData), &lightsData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void Renderer::refreshObjectBuffer()
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

  void Renderer::refreshObjectHandleBuffer()
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
  
  void Renderer::refreshMaterialBuffer()
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

  void Renderer::refreshInstancesBuffer()
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

  Handle<RenderMesh> Renderer::getMeshHandle(std::shared_ptr<Mesh> mesh)
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

  Handle<RenderMaterial> Renderer::getMaterialHandle(std::shared_ptr<Material> material)
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