#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include "../math/primitives.h"
#include "../scene/camera.h"
#include "../render/gpu_mesh.h"
#include "../render/gpu_texture.h"
#include "../render/texture_loader.h"
#include "../render/shader.h"
#include "../util/path_manager.h"

namespace Lotus
{
  class Terrain
  {
  public:
    static constexpr unsigned int ModelBinding = 0;
    static constexpr unsigned int ViewBinding = 1;
    static constexpr unsigned int ProjectionBinding = 2;
    static constexpr unsigned int LevelScaleBinding = 3;
    static constexpr unsigned int OffsetBinding = 4;
    static constexpr unsigned int HeightmapTextureBinding = 5;
    static constexpr unsigned int DebugColorBinding = 6;
    
    static constexpr unsigned int HeightmapTextureUnit = 0;

    Terrain(uint32_t levelsOfDetail = 7, uint32_t resolution = 4);

    void render(const Camera& camera);

  private:
    uint32_t levels;
    uint32_t tileResolution;

    ShaderProgram clipmapProgram;

    std::vector<std::shared_ptr<GPUMesh>> meshes;

    std::shared_ptr<GPUTexture> heightmapTexture;

    glm::mat4 rotationModels[4];

    glm::vec3 debugColors[5];
  };
}