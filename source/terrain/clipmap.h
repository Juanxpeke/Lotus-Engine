#pragma once

#include <cstdint>
#include <cmath>
#include "../math/primitives.h"
#include "../scene/camera.h"
#include "../render/gpu_mesh.h"
#include "../render/indirect/shader_program.h"
#include "../util/path_manager.h"

namespace Lotus
{
  class Clipmap
  {
  public:
    Clipmap(uint32_t clipmapLevels = 7, uint32_t clipmapTileResolution = 16, float clipmapQuadSize = 1.0f);

    void render(const Camera& camera);

  private:

    struct Tile : MeshPrimitive
    {
      Tile(uint32_t quadsPerSide, uint16_t quadSize)
      {
        uint32_t verticesPerSide = quadsPerSide + 1;

        vertices.reserve(verticesPerSide * verticesPerSide);
        
        for (uint32_t y = 0; y < verticesPerSide; y++)
        {
          for (uint32_t x = 0; x < verticesPerSide; x++)
          {
            Vertex vertex;
            vertex.position = { x * quadSize, 0, y * quadSize };

            vertices.push_back(vertex);
          }
        }

        indices.reserve(quadsPerSide * quadsPerSide * 6);

        for (uint32_t y = 0; y < quadsPerSide; y++)
        {
          for(uint32_t x = 0; x < quadsPerSide; x++)
          {
            indices.push_back(y * verticesPerSide + x);
            indices.push_back(y * verticesPerSide + x + 1);
            indices.push_back((y + 1) * verticesPerSide + x + 1);
            indices.push_back(y * verticesPerSide + x);
            indices.push_back((y + 1) * verticesPerSide + x + 1);
            indices.push_back((y + 1) * verticesPerSide + x);
          }
        }
      }

      unsigned int patch2D(unsigned int x, unsigned int y, unsigned int resolution)
      {
        return y * resolution + x;
      }
    };

    uint32_t levels;
    uint32_t tileResolution;
    uint16_t quadSize;

    ShaderProgram clipmapProgram;

    Tile tile;
    GPUMesh tileMesh;
  };
}