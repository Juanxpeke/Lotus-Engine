#pragma once

#include <memory>
#include <vector>
#include "../math/types.h"
#include "../scene/camera.h"
#include "../render/gpu_buffer.h"
#include "../render/gpu_mesh.h"
#include "../render/gpu_texture.h"
#include "../render/texture_loader.h"
#include "../render/shader.h"
#include "../util/path_manager.h"
#include "terrain.h"
#include "procedural_data_generator.h"

namespace Lotus
{
  class TerrainRenderer
  {
  friend class RenderingServer;

  public:
    static constexpr unsigned int ModelBinding = 0;

    static constexpr unsigned int LevelScaleBinding = 7;
    static constexpr unsigned int OffsetBinding = 8;
    static constexpr unsigned int HeightmapTextureArrayBinding = 9;

    static constexpr unsigned int DebugColorBinding = 10;
    
    static constexpr unsigned int HeightmapTextureUnit = 0;

    TerrainRenderer(uint32_t terrainLevels = 7, uint32_t terrainTileResolution = 128);

    void setLevels(uint32_t terrainLevels);
    void setTileResolution(uint32_t terrainTileResolution);
    
    std::shared_ptr<Terrain> createTerrain(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator);

    void render(const Camera& camera);

  private:

    // Buffers
    void fillProceduralBuffer();
    
    // Textures
    void updateHeightmapTextures(bool forced = false);

    uint32_t levels;
    uint32_t tileResolution;

    ShaderProgram clipmapProgram;

    std::vector<std::shared_ptr<GPUMesh>> meshes;
    std::shared_ptr<GPUArrayTexture> heightmapTextures;

    struct ProceduralData
    {
      unsigned int dataPerChunkSide;
      unsigned int chunksPerSide;
      glm::ivec2 dataOrigin;
      glm::uvec2 chunksOrigin;
    };

    UniformBuffer<ProceduralData> proceduralBuffer;
    
    glm::mat4 rotationModels[4];
    glm::vec3 debugColors[5];

    std::shared_ptr<Terrain> terrain;
  };
}