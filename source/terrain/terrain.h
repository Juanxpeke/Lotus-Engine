#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include "../math/primitives.h"
#include "../scene/camera.h"
#include "../render/gpu_buffer.h"
#include "../render/gpu_mesh.h"
#include "../render/gpu_texture.h"
#include "../render/texture_loader.h"
#include "../render/shader.h"
#include "../util/path_manager.h"
#include "procedural_data_generator.h"

namespace Lotus
{
  class Terrain
  {
  friend class RenderingServer;

  public:
    static constexpr unsigned int ModelBinding = 0;

    static constexpr unsigned int LevelScaleBinding = 7;
    static constexpr unsigned int OffsetBinding = 8;
    static constexpr unsigned int HeightmapTextureArrayBinding = 9;

    static constexpr unsigned int DebugColorBinding = 10;
    
    static constexpr unsigned int HeightmapTextureUnit = 0;

    Terrain(const std::shared_ptr<ProceduralDataGenerator>& dataGenerator, uint32_t levels = 7, uint32_t tileResolution = 128);

    void setDataGenerator(const std::shared_ptr<ProceduralDataGenerator>& dataGenerator);

    void render(const Camera& camera);

  private:

    // Buffers
    void fillProceduralBuffer();
    
    // Textures
    void updateHeightmapTextures(bool forced = false);

    uint32_t levels;
    uint32_t tileResolution;
    std::shared_ptr<ProceduralDataGenerator> dataGenerator;

    ShaderProgram clipmapProgram;

    std::vector<std::shared_ptr<GPUMesh>> meshes;
    std::shared_ptr<GPUArrayTexture> heightmapTextures;

    struct ProceduralData
    {
      int dataPerChunkSide;
      int chunksPerSide;
      glm::ivec2 dataOrigin;
      glm::ivec2 chunksOrigin;
    };

    UniformBuffer<ProceduralData> proceduralBuffer;
    
    glm::mat4 rotationModels[4];
    glm::vec3 debugColors[5];
  };
}