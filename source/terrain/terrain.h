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
#include "terrain_chunk_generator.h"

namespace Lotus
{
  class Terrain
  {
  public:
    static constexpr unsigned int ModelBinding = 0;
    static constexpr unsigned int ViewBinding = 1;
    static constexpr unsigned int ProjectionBinding = 2;

    static constexpr unsigned int DataPerChunkSideBinding = 3;
    static constexpr unsigned int ChunksPerSideBinding = 4;
    static constexpr unsigned int ChunksDataOrigin = 5;
    static constexpr unsigned int ChunksOrigin = 6;

    static constexpr unsigned int LevelScaleBinding = 7;
    static constexpr unsigned int OffsetBinding = 8;
    static constexpr unsigned int HeightmapTextureArrayBinding = 9;

    static constexpr unsigned int DebugColorBinding = 10;
    
    static constexpr unsigned int HeightmapTextureUnit = 0;

    Terrain(std::shared_ptr<ProceduralDataGenerator> chunkGenerator, uint32_t levelsOfDetail = 7, uint32_t resolution = 128);

    void setChunkGenerator(std::shared_ptr<ProceduralDataGenerator> chunkGenerator);

    void render(const Camera& camera);

  private:
    uint32_t levels;
    uint32_t tileResolution;

    ShaderProgram clipmapProgram;

    std::vector<std::shared_ptr<GPUMesh>> meshes;

    glm::mat4 rotationModels[4];

    glm::vec3 lastCameraPosition;
    bool firstTimeCamera = true;
    
    std::shared_ptr<ProceduralDataGenerator> chunkGenerator;
    
    std::shared_ptr<GPUTextureArray> heightmapTextures;

    glm::vec3 debugColors[5];
  };
}