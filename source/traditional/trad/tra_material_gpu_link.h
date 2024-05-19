#pragma once

#include <cstdint>
#include <memory>
#include "../material.h"
#include "../material_gpu_link.h"

class TraditionalMaterialGPULink : public MaterialGPULink
{
public:
  TraditionalMaterialGPULink() = delete;
  TraditionalMaterialGPULink(std::shared_ptr<Material> material);

  ~TraditionalMaterialGPULink();

private:
};