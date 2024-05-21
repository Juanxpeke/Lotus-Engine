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

  std::shared_ptr<MeshInstance> Renderer::createMeshInstance(std::shared_ptr<Mesh> mesh)
  {
    std::shared_ptr<MeshInstance> meshInstance = std::make_shared<MeshInstance>(mesh);

    meshInstances.push_back(meshInstance);

    RenderObject newObject;
    newObject.meshHandler = getMeshHandler(mesh);

    Handler<RenderObject> handler;
    handler.set(static_cast<uint32_t>(objects.size()));

    objects.push_back(newObject);

    dirtyObjectsHandlers.push_back(handler);
    unbatchedObjectsHandlers.push_back(handler);

    return meshInstance;
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
    
    updateObjects();

    buildBatches();

    refreshBuffers();

    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);

    for (int i = 0; i < shaderBatches.size(); i++)
    {
      const ShaderBatch& shaderBatch = shaderBatches[i];

      glUseProgram(shaders[shaderBatch.shaderHandler.get()].getProgramID());
      
      glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
      glUniformMatrix4fv(ProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

      glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*) 0, shaderBatch.count, sizeof(DrawElementsIndirectCommand));
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
  }

  void Renderer::updateObjects()
  {
    for (int i = 0; i < meshInstances.size(); i++)
    {
      const std::shared_ptr<MeshInstance>& meshInstance = meshInstances[i];
      Transform& meshTransform = meshInstance->transform;

      if (!meshTransform.dirty && !meshInstance->meshDirty && !meshInstance->materialDirty) { continue; }

      RenderObject& object = objects[i];
      Handler<RenderObject> objectHandler(i);

      if (meshTransform.dirty)
      {
        object.model = meshInstance->getModelMatrix();

        meshTransform.dirty = false;
      }
      if (meshInstance->meshDirty)
      {
        object.meshHandler = getMeshHandler(meshInstance->getMesh());

        toUnbatchObjectsHandlers.push_back(objectHandler);
        unbatchedObjectsHandlers.push_back(objectHandler);

        meshInstance->meshDirty = false;
      }
      if (meshInstance->materialDirty)
      {
        object.shaderHandler = Handler<int>();

        meshInstance->materialDirty = false;
      }

      dirtyObjectsHandlers.push_back(objectHandler);
    }
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
    if (toUnbatchObjectsHandlers.size() > 0)
    {
      std::vector<RenderBatch> deletionRenderBatches;
      deletionRenderBatches.reserve(toUnbatchObjectsHandlers.size());
      
      for (auto objectHandler : toUnbatchObjectsHandlers)
      {
        const RenderObject& object = objects[objectHandler.get()];
        RenderBatch batch;

        batch.objectHandler = objectHandler;
        batch.meshHandler = object.meshHandler;
        batch.shaderHandler = object.shaderHandler;

        deletionRenderBatches.push_back(batch);
      }

      toUnbatchObjectsHandlers.clear();

      std::sort(deletionRenderBatches.begin(), deletionRenderBatches.end(),
      [](const RenderBatch& bA, const RenderBatch& bB) {
        if (bA.shaderHandler < bB.shaderHandler) { return true; }
        else if (bA.shaderHandler == bB.shaderHandler) { return bA.meshHandler < bB.meshHandler; }
        else { return false; }
      });

      std::vector<RenderBatch> newRenderBatches;
      newRenderBatches.reserve(renderBatches.size());

      std::set_difference(renderBatches.begin(), renderBatches.end(), deletionRenderBatches.begin(), deletionRenderBatches.end(), std::back_inserter(newRenderBatches),
      [](const RenderBatch& bA, const RenderBatch& bB) {
        if (bA.shaderHandler< bB.shaderHandler) { return true; }
        else if (bA.shaderHandler == bB.shaderHandler) { return bA.meshHandler < bB.meshHandler; }
        else { return false; }
      });

      renderBatches = std::move(newRenderBatches);
    }

    std::vector<RenderBatch> newRenderBatches;
    newRenderBatches.reserve(unbatchedObjectsHandlers.size());
    
    // Fill new render batches
    for (auto objectHandler : unbatchedObjectsHandlers)
    {
      const RenderObject& object = objects[objectHandler.get()];
      RenderBatch batch;

      batch.objectHandler = objectHandler;		
      batch.meshHandler = object.meshHandler;
      batch.shaderHandler = object.shaderHandler;

      newRenderBatches.push_back(batch);
    }

    unbatchedObjectsHandlers.clear();
  
    // New render batches sort
    std::sort(newRenderBatches.begin(), newRenderBatches.end(),
    [](const RenderBatch& bA, const RenderBatch& bB)
    {
      if (bA.shaderHandler < bB.shaderHandler) { return true; }
      else if (bA.shaderHandler == bB.shaderHandler) { return bA.meshHandler < bB.meshHandler; }
      else { return false; }
    });
    
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
        if (bA.shaderHandler < bB.shaderHandler) { return true; }
        else if (bA.shaderHandler == bB.shaderHandler) { return bA.meshHandler < bB.meshHandler; }
        else { return false; }
      });
    }
    else if (renderBatches.size() == 0)
    {
      renderBatches = std::move(newRenderBatches);
    }
    
  }

  void Renderer::buildDrawBatches()
  {
    drawBatches.clear();
    
    if (renderBatches.size() == 0) { return; }
    
    DrawBatch newDrawBatch;
    newDrawBatch.prevInstanceCount = 0;
    newDrawBatch.instanceCount = 0;
    newDrawBatch.meshHandler = renderBatches[0].meshHandler;

    drawBatches.push_back(newDrawBatch);
    DrawBatch* backDrawBatch = &drawBatches.back();

    for (int i = 0; i < renderBatches.size(); i++)
    {
      RenderBatch* renderBatch = &renderBatches[i];

      bool bSameMesh = renderBatch->meshHandler.get() == backDrawBatch->meshHandler.get();
      bool bSameShader = renderBatch->shaderHandler.get() == backDrawBatch->shaderHandler.get();

      if (bSameMesh && bSameShader)
      {
        backDrawBatch->instanceCount++;
      }
      else
      {
        DrawBatch newDrawBatch;
        newDrawBatch.prevInstanceCount = i;
        newDrawBatch.instanceCount = 1;
        newDrawBatch.meshHandler = renderBatch->meshHandler;

        drawBatches.push_back(newDrawBatch);
        backDrawBatch = &drawBatches.back();
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
    newShaderBatch.shaderHandler = drawBatches[0].shaderHandler;

    shaderBatches.push_back(newShaderBatch);
    ShaderBatch* backShaderBatch = &shaderBatches.back();

    for (int i = 0; i < drawBatches.size(); i++)
    {
      DrawBatch* drawBatch = &drawBatches[i];

      bool bSameShader = drawBatch->shaderHandler.get() == backShaderBatch->shaderHandler.get();

      if (bSameShader)
      {
        backShaderBatch->count++;
      }
      else
      {
        ShaderBatch newShaderBatch;
        newShaderBatch.first = i;
        newShaderBatch.count = 1;
        newShaderBatch.shaderHandler = drawBatch->shaderHandler;

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
      if (indirectBufferAllocatedSize < drawBatches.size())
      {
        // reallocateBuffer(CPUIndirectBuffer, drawBatches.size() * sizeof(DrawElementsIndirectCommand));
      }

      for (int i = 0; i < drawBatches.size(); i++)
      {
        auto drawBatch = drawBatches[i];

        const DrawMesh& mesh = meshes[drawBatch.meshHandler.get()];

        CPUIndirectBuffer[i].count = mesh.count;
        CPUIndirectBuffer[i].instanceCount = drawBatch.instanceCount;
        CPUIndirectBuffer[i].firstIndex = mesh.firstIndex;
        CPUIndirectBuffer[i].baseVertex = mesh.baseVertex;
        CPUIndirectBuffer[i].baseInstance = drawBatch.prevInstanceCount;
      }

      glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);
      glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, drawBatches.size() * sizeof(DrawElementsIndirectCommand), CPUIndirectBuffer);
      glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
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
    if (dirtyObjectsHandlers.size() > 0)
    {
      size_t copySize = objects.size() * sizeof(GPUObjectData); // TODO
      if (objectBufferAllocatedSize < objects.size())
      {
        // reallocateBuffer(CPUObjectDataBuffer, copySize);
      }

      // If 80% of the objects are dirty, then just reupload the whole thing
      if (dirtyObjectsHandlers.size() >= 0) // TODO
      {
        for(int i = 0; i < objects.size(); i++)
        {
          const std::shared_ptr<MeshInstance> meshInstance = meshInstances[i];

          RenderObject* renderable = &objects[i];

          GPUObjectData object;
          object.model = meshInstance->getModelMatrix();

          memcpy(CPUObjectBuffer + i, &object, sizeof(GPUObjectData));
        }


      }
      else
      {
        // TODO
      }

      glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectBufferID);
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, objects.size() * sizeof(GPUObjectData), CPUObjectBuffer);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      dirtyObjectsHandlers.clear();
    }
  }

  void Renderer::refreshObjectHandleBuffer()
  {
    if (drawBatches.size() > 0)
    {
      if (objectHandleBufferAllocatedSize < objects.size()) // TODO
      {
        //reallocateBuffer(CPUObjectHandleBuffer, renderBatches.size() * sizeof(uint32_t));
      }

      int dataIndex = 0;
      for (int dI = 0; dI < drawBatches.size(); dI++)
      {
        auto drawBatch = drawBatches[dI];

        for (int iI = 0; iI < drawBatch.instanceCount; iI++)
        {
          CPUObjectHandleBuffer[dataIndex] = renderBatches[drawBatch.prevInstanceCount + iI].objectHandler.get();
          dataIndex++;
        }
      }

      glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectHandleBufferID);
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, objects.size() * sizeof(uint32_t), CPUObjectHandleBuffer);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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
          data[dataIndex].objectID = 0; //(&objects[renderBatches[drawBatch.prevInstancesCount + iI].objectHandler.get()])->objectHandler.get();
          data[dataIndex].drawBatchID = dI;
          dataIndex++;
        }
      }
    }
  }

}