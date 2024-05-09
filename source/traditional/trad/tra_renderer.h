#pragma once

#include <memory>
#include <vector>
#include "../renderer.h"

class TraditionalRenderer : public Renderer
{
public:
  TraditionalRenderer() = default;

  virtual void startUp() noexcept override;
  virtual void shutDown() noexcept override;

  virtual void render(Camera& camera) noexcept override;

  virtual std::shared_ptr<MeshInstance> createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) override;
  virtual std::shared_ptr<Material> createMaterial(MaterialType type) override;

private:
  std::vector<std::shared_ptr<MeshInstance>> meshInstances;

};