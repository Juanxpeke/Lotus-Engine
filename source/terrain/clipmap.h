#pragma once

#include <cstdint>
#include <cmath>
#include <memory>
#include "../math/primitives.h"
#include "../scene/camera.h"
#include "../render/gpu_mesh.h"
#include "../render/gpu_texture.h"
#include "../render/texture_loader.h"
#include "../render/shader.h"
#include "../util/path_manager.h"

namespace Lotus
{
  class Clipmap
  {
  public:
    static constexpr unsigned int ModelBinding = 0;
    static constexpr unsigned int ViewBinding = 1;
    static constexpr unsigned int ProjectionBinding = 2;
    static constexpr unsigned int LevelScaleBinding = 3;
    static constexpr unsigned int OffsetBinding = 4;
    static constexpr unsigned int HeightmapTextureBinding = 5;

    static constexpr unsigned int HeightmapTextureUnit = 0;


    Clipmap(uint32_t clipmapLevels = 7, uint32_t clipmapTileResolution = 16);

    void render(const Camera& camera);

  private:
    uint32_t levels;
    uint32_t tileResolution;

    ShaderProgram clipmapProgram;

    std::shared_ptr<GPUMesh> tileMesh;
    std::shared_ptr<GPUMesh> fillerMesh;
    std::shared_ptr<GPUMesh> trimMesh;

    std::shared_ptr<GPUTexture> heightmapTexture;

    glm::mat4 rotationModels[4];
  };
}