#pragma once

#include <cstdint>
#include <memory>
#include "i_mesh.h"
#include "i_material.h"
#include "i_mesh_instance.h"

class GraphicsBatch
{
friend class Renderer;

public:
  static constexpr int InitialAllocationInstancesCount = 1000;

  static constexpr int ModelsBindingPoint = 2;
  static constexpr int MaterialsBindingPoint = 3; 

  GraphicsBatch() = delete;
  GraphicsBatch(const GraphicsBatch& graphicsBatch) = delete;
  ~GraphicsBatch();

  uint32_t getMeshIndexCount() const noexcept;
  uint32_t getMeshVAO() const noexcept;
  uint32_t getShaderID() const noexcept;

  uint32_t getIndirectBufferID() const noexcept { return indirectBufferID; }
  uint32_t getModelBufferID() const noexcept { return modelBufferID; }
  uint32_t getMaterialBufferID() const noexcept { return materialBufferID; }

  void addMeshInstance(std::shared_ptr<MeshInstance>);
  void removeMeshInstance(std::shared_ptr<MeshInstance>);

private:
  GraphicsBatch(std::shared_ptr<Mesh> mesh, uint32_t shader);

  void updateBuffers() const;
  void updateIndirectBuffer() const;
  void updateModelBuffer() const;
  void updateMaterialBuffer() const;

  std::shared_ptr<Mesh> meshPtr;
  uint32_t shaderID;

  std::vector<std::shared_ptr<MeshInstance>> meshInstances;
  unsigned int allocatedInstancesCount;

  float* models;
  Material::MaterialData* materials;

  uint32_t indirectBufferID;
  uint32_t modelBufferID;
  uint32_t materialBufferID;
};