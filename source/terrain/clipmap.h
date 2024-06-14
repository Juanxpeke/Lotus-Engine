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
  struct Tile : MeshPrimitive
  {
    Tile(uint32_t quadsPerSide, uint16_t quadSize)
    {
      uint32_t verticesPerSide = quadsPerSide + 1;

      vertices.reserve(verticesPerSide * verticesPerSide);
      
      Vertex vertex;

      for (uint32_t y = 0; y < verticesPerSide; y++)
      {
        for (uint32_t x = 0; x < verticesPerSide; x++)
        {
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
  };

  struct Filler : MeshPrimitive
  {
    Filler(uint32_t quadsPerTileSide, uint16_t quadSize)
    {
      uint32_t verticesPerTileSide = quadsPerTileSide + 1;
      uint32_t offset = quadsPerTileSide;

      vertices.reserve(verticesPerTileSide * 8);

      Vertex vertex;
      
      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { (offset + i + 1) * quadSize, 0, 0 };
        vertices.push_back(vertex);
        vertex.position = { (offset + i + 1) * quadSize, 0, quadSize };
        vertices.push_back(vertex);
      }

      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { quadSize, 0, (offset + i + 1) * quadSize };
        vertices.push_back(vertex);
        vertex.position = { 0, 0, (offset + i + 1) * quadSize };
        vertices.push_back(vertex);
      }

      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { -float((offset + i) * quadSize), 0, quadSize };
        vertices.push_back(vertex);
        vertex.position = { -float((offset + i) * quadSize), 0, 0 };
        vertices.push_back(vertex);
      }

      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { 0, 0, -float((offset + i) * quadSize) };
        vertices.push_back(vertex);
        vertex.position = { quadSize, 0, -float((offset + i) * quadSize) };
        vertices.push_back(vertex);
      }

      indices.reserve(quadsPerTileSide * 24);

      for (uint32_t i = 0; i < quadsPerTileSide * 4; i++)
      {
        uint32_t arm = i / quadsPerTileSide;

        uint32_t bl = (arm + i) * 2 + 0;
        uint32_t br = (arm + i) * 2 + 1;
        uint32_t tl = (arm + i) * 2 + 2;
        uint32_t tr = (arm + i) * 2 + 3;

        if (arm % 2 == 0)
        {
          indices.push_back(br);
          indices.push_back(bl);
          indices.push_back(tr);
          indices.push_back(bl);
          indices.push_back(tl);
          indices.push_back(tr);
        }
        else
        {
          indices.push_back(br);
          indices.push_back(bl);
          indices.push_back(tl);
          indices.push_back(br);
          indices.push_back(tl);
          indices.push_back(tr);
        }
      }
    }
  };

  class Clipmap
  {
  public:
    Clipmap(uint32_t clipmapLevels = 7, uint32_t clipmapTileResolution = 2, float clipmapQuadSize = 1.0f);

    void render(const Camera& camera);

  private:
    uint32_t levels;
    uint32_t tileResolution;
    uint16_t quadSize;

    ShaderProgram clipmapProgram;

    Tile tile;
    Filler filler;
    GPUMesh tileMesh;
    GPUMesh fillerMesh;
  };
}