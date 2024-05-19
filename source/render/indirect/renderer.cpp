#include "renderer.h"

#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../path_manager.h"

namespace Lotus {

  Renderer::Renderer() :
    vertexArrayID(0),
    vertexBufferAllocatedSize(VertexBufferInitialAllocationSize),
    vertexBufferSize(0),
    indexBufferAllocatedSize(IndexBufferInitialAllocationSize),
    indexBufferSize(0),
    indirectBufferAllocatedSize(IndirectBufferInitialAllocationSize),
    indirectBufferSize(0),
    objectBufferAllocatedSize(ObjectBufferInitialAllocationSize),
    objectBufferSize(0),
    materialBufferAllocatedSize(MaterialBufferInitialAllocationSize),
    materialBufferSize(0),
    ambientLight({1.0, 1.0, 1.0})
  {}

  void Renderer::startUp()
  {
    shaders[static_cast<unsigned int>(0)] = ShaderProgram(shaderPath("n_diffuse_flat.vert"), shaderPath("n_diffuse_flat.frag"));

    glEnable(GL_DEPTH_TEST);
    
    glGenVertexArrays(1, &vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glGenBuffers(1, &indexBufferID);
    glGenBuffers(1, &indirectBufferID);
    glGenBuffers(1, &lightBufferID);
    glGenBuffers(1, &objectBufferID);
    glGenBuffers(1, &materialBufferID);
    glGenBuffers(1, &objectHandleBufferID);

    glBindVertexArray(vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferAllocatedSize * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferAllocatedSize * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

    CPUIndirectBuffer = new DrawElementsIndirectCommand[indirectBufferAllocatedSize];
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, indirectBufferAllocatedSize * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, lightBufferID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPULightsData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, LightUBOBindingPoint, lightBufferID);

    CPUObjectBuffer = new GPUObjectData[objectBufferAllocatedSize];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectBufferID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, objectBufferAllocatedSize * sizeof(GPUObjectData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ObjectSSBOBindingPoint, objectBufferID);

    CPUMaterialBuffer = new GPUMaterialData[materialBufferAllocatedSize];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialBufferID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, materialBufferAllocatedSize * sizeof(GPUMaterialData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MaterialSSBOBindingPoint, materialBufferID);

    CPUObjectHandleBuffer = new uint32_t[objectBufferAllocatedSize];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectHandleBufferID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, objectBufferAllocatedSize * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ObjectHandleSSBOBindingPoint, objectHandleBufferID);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glfwSwapInterval(0);
  }

  Renderer::~Renderer()
  {
    if (vertexArrayID)
    {
      glDeleteBuffers(1, &vertexBufferID);
      glDeleteBuffers(1, &indexBufferID);
      glDeleteBuffers(1, &indirectBufferID);
      glDeleteBuffers(1, &lightBufferID);
      glDeleteBuffers(1, &objectBufferID);
      glDeleteBuffers(1, &materialBufferID);
      glDeleteBuffers(1, &objectHandleBufferID);
      glDeleteVertexArrays(1, &vertexArrayID);

      vertexArrayID = 0;

      delete[] CPUIndirectBuffer;
      delete[] CPUObjectBuffer;
      delete[] CPUMaterialBuffer;
      delete[] CPUObjectHandleBuffer;
    }
  }

  Handler<RenderObject> Renderer::createObject(std::shared_ptr<Mesh> mesh)
  {
    RenderObject newObject;
    newObject.meshHandler = getMeshHandler(mesh);
    // newObj.model = object->model;	
    // newObj.updateIndex = (uint32_t)-1;
    // newObj.index = -1;

    Handler<RenderObject> handler;
    handler.set(static_cast<uint32_t>(renderables.size()));

    renderables.push_back(newObject);
    unbatchedObjectsHandlers.push_back(handler);

    updateObject(handler);

    return handler;
  }

  Handler<DrawMesh> Renderer::getMeshHandler(std::shared_ptr<Mesh> mesh)
  {
    Handler<DrawMesh> handler;

    auto it = meshMap.find(mesh);

    if (it == meshMap.end())
    {
      const std::vector<Vertex>& vertices = mesh->getVertices();
      const std::vector<unsigned int>& indices = mesh->getIndices();

      DrawMesh newMesh;
      newMesh.firstIndex = indexBufferSize;
      newMesh.baseVertex = vertexBufferSize;
      newMesh.count = indices.size();

      glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
      glBufferSubData(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(Vertex), vertices.size() * sizeof(Vertex), vertices.data());
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      vertexBufferSize += vertices.size();

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize * sizeof(unsigned int), indices.size() * sizeof(unsigned int), indices.data());
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      indexBufferSize += indices.size();

      handler.set(static_cast<uint32_t>(meshes.size()));
      meshMap[mesh] = handler;
      
      meshes.push_back(newMesh);
    }
    else
    {
      handler = (*it).second;
    }
    return handler;
  }

  void Renderer::updateObject(Handler<RenderObject> objectHandler)
  {

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
    
    // buildBatches();

    // refresh();





    CPUIndirectBuffer[0].count = meshes[0].count;
    CPUIndirectBuffer[0].instanceCount = 1;
    CPUIndirectBuffer[0].firstIndex = 0;
    CPUIndirectBuffer[0].baseVertex = 0;
    CPUIndirectBuffer[0].baseInstance = 0;
    
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, 1 * sizeof(DrawElementsIndirectCommand), CPUIndirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);

    glUseProgram(shaders[0].getProgramID());
    glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(6, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    
    for (int i = 0; i < 1; i++)
    {
      glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*) 0, 1, sizeof(DrawElementsIndirectCommand));
    }
  }

  void Renderer::refreshBuffers()
  {
    refreshObjectBuffer();

		if (indirectBufferAllocatedSize < drawBatches.size())
		{
			// reallocateBuffer(CPUIndirectBuffer, drawBatches.size() * sizeof(DrawElementsIndirectCommand));
		}

		if (0 < renderBatches.size())
		{
			// reallocateBuffer(CPU_GPUInstanceBuffer, renderBatches.size() * sizeof(GPUInstance));
		}

		if (objectHandleBufferAllocatedSize < renderBatches.size())
		{
			//reallocateBuffer(CPUObjectHandleBuffer, renderBatches.size() * sizeof(uint32_t));
		}

    if (drawBatches.size() > 0)
    {
      fillIndirectBuffer();
    }

    if (renderBatches.size() > 0)
    {
      // fillInstancesBuffer();
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
    if (dirtyObjects.size() > 0)
    {
      size_t copySize = renderables.size() * sizeof(GPUObjectData);
      if (objectBufferAllocatedSize < renderables.size())
      {
        // reallocateBuffer(CPUObjectDataBuffer, copySize);
      }

      // If 80% of the objects are dirty, then just reupload the whole thing
      if (dirtyObjects.size() >= renderables.size() * 0.8)
      {
        fillObjectBuffer();
      }
    }
  }

  void Renderer::buildBatches()
  {
    std::vector<uint32_t> newObjectsHandles;
    {
      // Fill ObjectList
      newObjectsHandles.reserve(unbatchedObjectsHandlers.size());
      for (auto objectHandler : unbatchedObjectsHandlers)
      {
        PassObject newObject;

        // newObject.original = objectHandler;
        // newObject.meshID = &renderables[objectHandler]->meshID;
        // newObject.customKey = &renderables[objectHandler]->customSortKey;

        uint32_t handle = -1;

        // Reuse handle
        if (reusableObjects.size() > 0)
        {
          handle = reusableObjects.back().get();
          reusableObjects.pop_back();
          objects[handle] = newObject;
        }
        else 
        {
          handle = objects.size();
          objects.push_back(newObject);
        }

        newObjectsHandles.push_back(handle);
        // &renderables[objectHandler]->index = static_cast<uint32_t>(handle);
      }

      unbatchedObjectsHandlers.clear();
    }

    std::vector<RenderBatch> newRenderBatches;
    newRenderBatches.reserve(newObjectsHandles.size());

    {
      // Fill DrawList	

      for (auto i : newObjectsHandles)
      {
        RenderBatch newCommand;

        auto obj = objects[i];
        newCommand.objectHandler.set(i);

        // Pack mesh ID and material into 64 bits				
        newCommand.sortKey = 0;

        newRenderBatches.push_back(newCommand);
      }
    }
    {
      // Draw Sort
      std::sort(newRenderBatches.begin(), newRenderBatches.end(),
      [](const RenderBatch& bA, const RenderBatch& bB)
      {
        if (bA.sortKey < bB.sortKey) { return true; }
        else if (bA.sortKey == bB.sortKey) { return bA.objectHandler.get() < bB.objectHandler.get(); }
        else { return false; }
      });
    }
    {
      // Merge the new batches into the main render batch array
      if (renderBatches.size() > 0 && newRenderBatches.size() > 0)
      {
      }
      else if (renderBatches.size() == 0)
      {
        renderBatches = std::move(newRenderBatches);
      }
    }
    {
      if (renderBatches.size() == 0) return;
      
      // Draw merge
      buildDrawBatches();

      // Shader merge
      buildShaderBatches();
    }
  }

  void Renderer::buildDrawBatches()
  {
    drawBatches.clear();

    DrawBatch newDrawBatch;
    newDrawBatch.prevInstancesCount = 0;
    newDrawBatch.instancesCount = 0;

    newDrawBatch.meshHandler = (&objects[renderBatches[0].objectHandler.get()])->meshHandler;

    drawBatches.push_back(newDrawBatch);
    DrawBatch* backDrawBatch = &drawBatches.back();

    for (int i = 0; i < renderBatches.size(); i++)
    {
      PassObject* obj = &objects[renderBatches[i].objectHandler.get()];

      bool bSameMesh = obj->meshHandler.get() == backDrawBatch->meshHandler.get();

      if (bSameMesh)
      {
        backDrawBatch->instancesCount++;
      }
      else
      {
        DrawBatch newDrawBatch;
        newDrawBatch.prevInstancesCount = i;
        newDrawBatch.instancesCount = 1;
        newDrawBatch.meshHandler = obj->meshHandler;

        drawBatches.push_back(newDrawBatch);
        backDrawBatch = &drawBatches.back();
      }
    }
  }

  void Renderer::buildShaderBatches()
  {
    shaderBatches.clear();

    ShaderBatch newShaderBatch;

    newShaderBatch.count = 1;
    newShaderBatch.first = 0;

    for (int i = 1; i < drawBatches.size(); i++)
    {
      DrawBatch* joinbatch = &drawBatches[newShaderBatch.first];
      DrawBatch* batch = &drawBatches[i];

      bool bSameShader = false;

      /*
      if (joinbatch->material.shaderID == batch->material.shaderID)
      {
        bSameShader = true;
      }*/

      if (!bSameShader)
      {
        shaderBatches.push_back(newShaderBatch);
        newShaderBatch.count = 1;
        newShaderBatch.first = i;
      }
      else
      {
        newShaderBatch.count++;
      }
    }
  }

  void Renderer::fillObjectBuffer()
  {
    for(int i = 0; i < renderables.size(); i++)
    {
      RenderObject* renderable = &renderables[i];
      GPUObjectData object;
      object.model = renderable->model;

      memcpy(CPUObjectBuffer + i, &object, sizeof(GPUObjectData));
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectBufferID);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, renderables.size(), CPUObjectBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void Renderer::fillIndirectBuffer()
  {
    for (int i = 0; i < drawBatches.size(); i++)
    {
      auto drawBatch = drawBatches[i];

      const DrawMesh& mesh = meshes[drawBatch.meshHandler.get()];

      CPUIndirectBuffer[i].count = mesh.count;
      CPUIndirectBuffer[i].instanceCount = drawBatch.instancesCount;
      CPUIndirectBuffer[i].firstIndex = mesh.firstIndex;
      CPUIndirectBuffer[i].baseVertex = mesh.baseVertex;
      CPUIndirectBuffer[i].baseInstance = drawBatch.prevInstancesCount;
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, drawBatches.size(), CPUIndirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  }

  void Renderer::fillInstancesBuffer(GPUInstance* data)
  {
    int dataIndex = 0;
    for (int dI = 0; dI < drawBatches.size(); dI++)
    {
      auto drawBatch = drawBatches[dI];

      for (int iI = 0; iI < drawBatch.instancesCount; iI++)
      {
        data[dataIndex].objectID = 0; //(&objects[renderBatches[drawBatch.prevInstancesCount + iI].objectHandler.get()])->objectHandler.get();
        data[dataIndex].drawBatchID = dI;
        dataIndex++;
      }
    }
  }

}