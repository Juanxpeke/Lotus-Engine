#include "renderer.h"

#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../util/path_manager.h"

namespace Lotus {

  void printRenderBatches(const std::vector<RenderBatch>& batches)
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

    GPUObjectData gpuObject;
    gpuObject.model = meshInstance->getModelMatrix();
    gpuObject.materialHandle = getMaterialHandle(material).get();
      
    uint32_t objectID = GPUObjectBuffer.add(&gpuObject);

    RenderObject newObject;
    newObject.model = gpuObject.model;
    newObject.meshHandle = getMeshHandle(mesh);
    newObject.materialHandle = getMaterialHandle(material);
    newObject.ID = objectID;
    
    Handle<RenderObject> handle(static_cast<uint32_t>(objects.size()));
    objects.push_back(newObject);
    
    //GPUObjectBuffer.filledSize++;
    //GPUObjectHandleBuffer.filledSize++;
    uint32_t xd = 1;
    GPUObjectHandleBuffer.add(&xd);


    //dirtyObjectsHandles.push_back(handle);
    unbatchedObjectsHandlers.push_back(handle);

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

#if 1    
    updateObjects();
    updateMaterials();

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

    glBindVertexArray(0);
#else
#endif
  }

  void Renderer::updateObjects()
  {
    for (int i = 0; i < meshInstances.size(); i++)
    {
      const std::shared_ptr<MeshInstance>& meshInstance = meshInstances[i];
      Transform* transform = &(meshInstance->transform);

      if (transform->dirty || meshInstance->meshDirty || meshInstance->materialDirty)
      {
        RenderObject& object = objects[i];
        Handle<RenderObject> objectHandle(i);

        if (transform->dirty)
        {
          object.model = meshInstance->getModelMatrix();
          transform->dirty = false;
        }
        if (meshInstance->materialDirty)
        {
          object.materialHandle = getMaterialHandle(meshInstance->getMaterial());
          meshInstance->materialDirty = false;
        }
        if (meshInstance->meshDirty || meshInstance->shaderDirty)
        {
          /*
            The renderer MUST rearrange the batch related to this object if the mesh or the shader change
            so the batches ordering logic works accordingly.
          */
          toUnbatchObjects.push_back(object);
          unbatchedObjectsHandlers.push_back(objectHandle);

          object.meshHandle = getMeshHandle(meshInstance->getMesh());
          object.shaderID = 0;// meshInstance->getMaterial()->getShaderID();
          
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
    GPUMaterialData* materialBuffer = GPUMaterialBuffer.map();
    for (int i = 0; i < materials.size(); i++)
    {
      const std::shared_ptr<Material>& material = materials[i];

      if (material->dirty)
      {
        materialBuffer[i] = material->getMaterialData();
        material->dirty = false;
      }
    }
    GPUMaterialBuffer.unmap();
  }

  void Renderer::buildBatches()
  {
    // Render merge
    buildRenderBatches();

    // Draw merge
    buildDrawBatches();

    // Shader merge
    buildShaderBatches();
  }

  void Renderer::buildRenderBatches()
  {
    if (toUnbatchObjects.size() > 0)
    {
      std::cout << "Render batches" << std::endl;
      printRenderBatches(renderBatches);

      std::vector<RenderBatch> deletionRenderBatches;
      deletionRenderBatches.reserve(toUnbatchObjects.size());
      
      for (auto object : toUnbatchObjects)
      {
        RenderBatch batch;

        batch.objectHandle = Handle<RenderObject>(object.ID);
        batch.meshHandle = object.meshHandle;
        batch.shaderHandle = object.shaderID;

        deletionRenderBatches.push_back(batch);
      }

      std::cout << "Deletion render batches" << std::endl;
      printRenderBatches(deletionRenderBatches);

      toUnbatchObjects.clear();

      std::sort(deletionRenderBatches.begin(), deletionRenderBatches.end(),
      [](const RenderBatch& bA, const RenderBatch& bB) {
        if (bA.shaderHandle < bB.shaderHandle) { return true; }
        else if (bA.shaderHandle == bB.shaderHandle) {
                                if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
        }
        else { return false; }
      });

      std::cout << "Ordered deletion render batches" << std::endl;
      printRenderBatches(deletionRenderBatches);

      std::vector<RenderBatch> newRenderBatches;
      newRenderBatches.reserve(renderBatches.size());

      std::set_difference(renderBatches.begin(), renderBatches.end(), deletionRenderBatches.begin(), deletionRenderBatches.end(), std::back_inserter(newRenderBatches),
      [](const RenderBatch& bA, const RenderBatch& bB) {
        if (bA.shaderHandle < bB.shaderHandle) { return true; }
        else if (bA.shaderHandle == bB.shaderHandle) {
                                if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
        }
        else { return false; }
      });

      renderBatches = std::move(newRenderBatches);

      std::cout << "Render batches after deletion" << std::endl;
      printRenderBatches(renderBatches);
    }

    if (unbatchedObjectsHandlers.size() > 0)
    {
      std::vector<RenderBatch> newRenderBatches;
      newRenderBatches.reserve(unbatchedObjectsHandlers.size());
      
      // Fill new render batches
      for (auto objectHandle : unbatchedObjectsHandlers)
      {
        const RenderObject& object = objects[objectHandle.get()];
        RenderBatch batch;

        batch.objectHandle = object.ID;		
        batch.meshHandle = object.meshHandle;
        batch.shaderHandle = object.shaderID;

        newRenderBatches.push_back(batch);
      }

      // std::cout << "New render batches" << std::endl;
      // printRenderBatches(newRenderBatches);

      unbatchedObjectsHandlers.clear();

      // New render batches sort
      std::sort(newRenderBatches.begin(), newRenderBatches.end(),
      [](const RenderBatch& bA, const RenderBatch& bB)
      {
        if (bA.shaderHandle < bB.shaderHandle) { return true; }
        else if (bA.shaderHandle == bB.shaderHandle) {
                      if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
        }
        else { return false; }
      });

      // std::cout << "Ordered new render batches" << std::endl;
      // printRenderBatches(newRenderBatches);

      // Merge the new render batches into the main render batch array
      if (renderBatches.size() > 0 && newRenderBatches.size() > 0)
      {
        int index = renderBatches.size();
        renderBatches.reserve(renderBatches.size() + newRenderBatches.size());
        
        for (auto b : newRenderBatches)
        {
          renderBatches.push_back(b);
        }

        RenderBatch* begin = renderBatches.data();
        RenderBatch* mid = begin + index;
        RenderBatch* end = begin + renderBatches.size();

        std::inplace_merge(begin, mid, end,
        [](const RenderBatch& bA, const RenderBatch& bB) {
          if (bA.shaderHandle < bB.shaderHandle) { return true; }
          else if (bA.shaderHandle == bB.shaderHandle) {
            if (bA.meshHandle < bB.meshHandle) { return true; }
            else if (bA.meshHandle == bB.meshHandle) { return bA.objectHandle < bB.objectHandle; }
            else { return false; }
          }
          else { return false; }
        });
      }
      else if (renderBatches.size() == 0)
      {
        renderBatches = std::move(newRenderBatches);
      }

      // std::cout << "Render batches after addition" << std::endl;
      // printRenderBatches(renderBatches);
    }
  }

  void Renderer::buildDrawBatches()
  {
    drawBatches.clear();
    GPUIndirectBuffer.filledSize = 0;
    
    if (renderBatches.size() == 0) { return; }
    
    DrawBatch newDrawBatch;
    newDrawBatch.prevInstanceCount = 0;
    newDrawBatch.instanceCount = 0;
    newDrawBatch.meshHandle = renderBatches[0].meshHandle;

    drawBatches.push_back(newDrawBatch);
    DrawBatch* backDrawBatch = &drawBatches.back();

    GPUIndirectBuffer.filledSize++;

    for (int i = 0; i < renderBatches.size(); i++)
    {
      RenderBatch* renderBatch = &renderBatches[i];

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
  }

  void Renderer::refreshIndirectBuffer()
  {
    if (drawBatches.size() > 0)
    {
      DrawElementsIndirectCommand* indirectBuffer = GPUIndirectBuffer.map();

      for (int i = 0; i < drawBatches.size(); i++)
      {
        auto drawBatch = drawBatches[i];

        const RenderMesh& mesh = meshes[drawBatch.meshHandle.get()];

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
    if (!dirtyObjectsHandles.empty())
    {
      GPUObjectData* objectBuffer = GPUObjectBuffer.map();
      
      for (const Handle<RenderObject>& objectHandle : dirtyObjectsHandles)
      {
        const RenderObject& object = objects[objectHandle.get()];

        objectBuffer[object.ID].model = object.model;
        objectBuffer[object.ID].materialHandle = object.materialHandle.get();
      }

      GPUObjectBuffer.unmap();

      dirtyObjectsHandles.clear();
    }
  }

  void Renderer::refreshObjectHandleBuffer()
  {
    if (drawBatches.size() > 0)
    {
      uint32_t* objectHandleBuffer = GPUObjectHandleBuffer.map();

      int index = 0;
      for (int dI = 0; dI < drawBatches.size(); dI++)
      {
        auto drawBatch = drawBatches[dI];

        for (int iI = 0; iI < drawBatch.instanceCount; iI++)
        {
          objectHandleBuffer[index] = objects[renderBatches[drawBatch.prevInstanceCount + iI].objectHandle.get()].ID;
          index++;
        }
      }

      GPUObjectHandleBuffer.unmap();
    }
  }

  void Renderer::refreshInstancesBuffer()
  {
		if (0 < renderBatches.size())
		{
			// reallocateBuffer(CPU_GPUInstanceBuffer, renderBatches.size() * sizeof(GPUInstance));
		}

    if (renderBatches.size() > 0)
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

      RenderMesh newMesh;
      newMesh.firstIndex = indicesBufferLocation;
      newMesh.baseVertex = verticesBufferLocation;
      newMesh.count = indices.size();

      handle.set(static_cast<uint32_t>(meshes.size()));
      meshMap[mesh] = handle;
      
      meshes.push_back(newMesh);
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
      handle.set(static_cast<uint32_t>(materials.size()));
      materialMap[material] = handle;
      
      materials.push_back(material);
      GPUMaterialBuffer.filledSize++;
    }
    else
    {
      handle = (*it).second;
    }

    return handle;
  }

}