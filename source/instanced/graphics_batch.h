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
  static constexpr int INITIAL_INSTANCES_COUNT = 2000;

  static constexpr int MODELS_BINDING_POINT = 2;
  static constexpr int MATERIAL_BINDING_POINT = 3; 

  GraphicsBatch() = delete;

  uint32_t getMeshIndexCount() const noexcept;
  uint32_t getMeshVAO() const noexcept;
  uint32_t getShaderID() const noexcept;

  uint32_t getIndirectBufferID() const noexcept { return indirectBufferID; }
  uint32_t getModelBufferID() const noexcept { return modelBufferID; }
  uint32_t getMaterialBufferID() const noexcept { return materialBufferID; }

private:
  GraphicsBatch(std::shared_ptr<Mesh> mesh, uint32_t shader);

  void updateBuffers() const;
  void updateIndirectBuffer() const;
  void updateModelBuffer() const;

  std::shared_ptr<Mesh> meshPtr;
  uint32_t shaderID;

  std::vector<MeshInstance> meshInstances;

  uint32_t indirectBufferID;
  uint32_t modelBufferID;
  uint32_t materialBufferID;
};