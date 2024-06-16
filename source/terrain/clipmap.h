#pragma once

#include <cstdint>
#include <cmath>
#include "../math/primitives.h"
#include "../scene/camera.h"
#include "../render/gpu_mesh.h"
#include "../render/shader.h"
#include "../util/path_manager.h"

namespace Lotus
{
  struct Tile : MeshPrimitive
  {
    Tile(uint32_t quadsPerSide)
    {
      uint32_t verticesPerSide = quadsPerSide + 1;

      vertices.reserve(verticesPerSide * verticesPerSide);
      
      Vertex vertex;

      for (uint32_t y = 0; y < verticesPerSide; y++)
      {
        for (uint32_t x = 0; x < verticesPerSide; x++)
        {
          vertex.position = { x, 0, y };
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
    Filler(uint32_t quadsPerTileSide)
    {
      uint32_t verticesPerTileSide = quadsPerTileSide + 1;
      uint32_t offset = quadsPerTileSide;

      vertices.reserve(verticesPerTileSide * 8);

      Vertex vertex;
      
      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { offset + i + 1, 0, 0 };
        vertices.push_back(vertex);
        vertex.position = { offset + i + 1, 0, 1 };
        vertices.push_back(vertex);
      }

      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { 1, 0, offset + i + 1 };
        vertices.push_back(vertex);
        vertex.position = { 0, 0, offset + i + 1 };
        vertices.push_back(vertex);
      }

      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { -float(offset + i), 0, 1 };
        vertices.push_back(vertex);
        vertex.position = { -float(offset + i), 0, 0 };
        vertices.push_back(vertex);
      }

      for (uint32_t i = 0; i < verticesPerTileSide; i++)
      {
        vertex.position = { 0, 0, -float(offset + i) };
        vertices.push_back(vertex);
        vertex.position = { 1, 0, -float(offset + i) };
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

  struct Trim : MeshPrimitive
  {
    Trim(uint32_t quadsPerTileSide)
    {
      uint32_t quadsPerLevelSide = quadsPerTileSide * 4 + 1;
      uint32_t verticesPerLevelSide = quadsPerLevelSide + 1;

      vertices.reserve((verticesPerLevelSide * 2 + 1) * 2);

      Vertex vertex;

      // Vertical part of L
      for (uint32_t i = 0; i < verticesPerLevelSide + 1; i++)
      {
        vertex.position = { 0, 0, verticesPerLevelSide - i };
        vertices.push_back(vertex);
        vertex.position = { 1, 0, verticesPerLevelSide - i };
        vertices.push_back(vertex);
      }

      // Horizontal part of L
      for (uint32_t i = 0; i < verticesPerLevelSide; i++)
      {
        vertex.position = { i + 1, 0, 0 };
        vertices.push_back(vertex);
        vertex.position = { i + 1, 0, 1 };
        vertices.push_back(vertex);
      }

      for (Vertex& v : vertices)
      {
        v.position -= glm::vec3(0.5f * (verticesPerLevelSide + 1), 0, 0.5f * (verticesPerLevelSide + 1));
      }
      
      indices.reserve((verticesPerLevelSide * 2 - 1) * 6);

      for (uint32_t i = 0; i < verticesPerLevelSide; i++)
      {
        indices.push_back((i + 0) * 2 + 1);
        indices.push_back((i + 0) * 2 + 0);
        indices.push_back((i + 1) * 2 + 0);
        indices.push_back((i + 1) * 2 + 1);
        indices.push_back((i + 0) * 2 + 1);
        indices.push_back((i + 1) * 2 + 0);
      }

      uint32_t startOfHorizontal = (verticesPerLevelSide + 1) * 2;

      for (uint32_t i = 0; i < verticesPerLevelSide - 1; i++)
      {
        indices.push_back(startOfHorizontal + (i + 0) * 2 + 1);
        indices.push_back(startOfHorizontal + (i + 0) * 2 + 0);
        indices.push_back(startOfHorizontal + (i + 1) * 2 + 0);
        indices.push_back(startOfHorizontal + (i + 1) * 2 + 1);
        indices.push_back(startOfHorizontal + (i + 0) * 2 + 1);
        indices.push_back(startOfHorizontal + (i + 1) * 2 + 0);
      }
    }
  };

  class Clipmap
  {
  public:
    static constexpr unsigned int ModelBinding = 0;
    static constexpr unsigned int ViewBinding = 1;
    static constexpr unsigned int ProjectionBinding = 2;
    static constexpr unsigned int LevelScaleBinding = 3;
    static constexpr unsigned int OffsetBinding = 4;


    Clipmap(uint32_t clipmapLevels = 7, uint32_t clipmapTileResolution = 2);

    void render(const Camera& camera);

  private:
    uint32_t levels;
    uint32_t tileResolution;

    ShaderProgram clipmapProgram;

    Tile tile;
    Filler filler;
    Trim trim;
    GPUMesh tileMesh;
    GPUMesh fillerMesh;
    GPUMesh trimMesh;

    glm::mat4 rotationModels[4];
  };
}