#include "graphics_batch.h"

#include <glad/glad.h>

struct DrawElementsIndirectCommand
{
  uint32_t count = 0;         // # of elements (i.e indices)
  uint32_t instanceCount = 0; // # of instances (kind of draw calls)
  uint32_t firstIndex = 0;    // Index of the first element in the EBO
  uint32_t baseVertex = 0;    // Base vertex when reading from vertex buffer
  uint32_t baseInstance = 0;  // Base instance when using gl_InstanceID
  uint32_t padding0 = 0;      // Padding due to GLSL layout std140 16 bytes alignment rule
  uint32_t padding1 = 0;
  uint32_t padding2 = 0;
};

GraphicsBatch::GraphicsBatch(std::shared_ptr<Mesh> mesh, uint32_t shader) :
  meshPtr(mesh), shaderID(shader), allocatedInstancesCount(InitialAllocationInstancesCount)
{
  unsigned int IBO, modelSSBO, materialSSBO;
  
  glGenBuffers(1, &IBO);
  glGenBuffers(1, &modelSSBO);
  glGenBuffers(1, &materialSSBO);

  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, 1 * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_DRAW);

  models = new float[allocatedInstancesCount * 16];
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, allocatedInstancesCount * 16 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  materials = new Material::MaterialData[allocatedInstancesCount];
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, allocatedInstancesCount * sizeof(Material::MaterialData), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  indirectBufferID = IBO;
  modelBufferID = modelSSBO;
  materialBufferID = materialSSBO;
}

GraphicsBatch::~GraphicsBatch()
{
  delete[] models;
  delete[] materials;
}

uint32_t GraphicsBatch::getMeshIndexCount() const noexcept
{
  return meshPtr->getIndexBufferCount();
}

uint32_t GraphicsBatch::getMeshVAO() const noexcept
{
  return meshPtr->getVertexArrayID();
}

uint32_t GraphicsBatch::getShaderID() const noexcept
{
  return shaderID;
}

void GraphicsBatch::addMeshInstance(std::shared_ptr<MeshInstance> meshInstance)
{
  meshInstances.push_back(meshInstance);
}

void GraphicsBatch::removeMeshInstance(std::shared_ptr<MeshInstance> meshInstance)
{
  std::erase(meshInstances, meshInstance);
}

void GraphicsBatch::updateBuffers() const
{
  updateIndirectBuffer();
  updateModelBuffer();
  updateMaterialBuffer();
}

void GraphicsBatch::updateIndirectBuffer() const
{
  DrawElementsIndirectCommand drawCommands[1];

  drawCommands[0].count = getMeshIndexCount();
  drawCommands[0].instanceCount = meshInstances.size();
  drawCommands[0].firstIndex = 0;
  drawCommands[0].baseVertex = 0;
  drawCommands[0].baseInstance = 0;

  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBufferID);
  glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, 1 * sizeof(DrawElementsIndirectCommand), drawCommands);
}

void GraphicsBatch::updateModelBuffer() const
{
  unsigned int instancesCount = meshInstances.size() > allocatedInstancesCount ? allocatedInstancesCount : meshInstances.size();

  for (int i = 0; i < instancesCount; i++)
  {
    const std::shared_ptr<MeshInstance> meshInstance = meshInstances[i];

    glm::mat4 model = meshInstance->getModelMatrix();
    const float* modelPtr = glm::value_ptr(model);

    std::copy(modelPtr, modelPtr + 16, models + i * 16);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelBufferID);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, instancesCount * 16 * sizeof(float), models);
}

void GraphicsBatch::updateMaterialBuffer() const
{
  unsigned int instancesCount = meshInstances.size() > allocatedInstancesCount ? allocatedInstancesCount : meshInstances.size();

  for (int i = 0; i < instancesCount; i++)
  {
    const std::shared_ptr<MeshInstance> meshInstance = meshInstances[i];
    meshInstance->getMaterial()->fillMaterialData(materials[i]);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialBufferID);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, instancesCount * sizeof(Material::MaterialData), materials);
}


