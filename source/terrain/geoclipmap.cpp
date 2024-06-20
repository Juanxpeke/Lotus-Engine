#include "geoclipmap.h"

#include "../util/log.h"

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

  struct Cross : MeshPrimitive
  {
    Cross(uint32_t quadsPerTileSide)
    {
      uint32_t verticesPerTileSide = quadsPerTileSide + 1;

      vertices.reserve(verticesPerTileSide * 8);
      
      Vertex vertex;

      // Horizontal vertices
      for (uint32_t i = 0; i < verticesPerTileSide * 2; i++)
      {
        vertex.position = { i - float(quadsPerTileSide), 0, 0 };
        vertices.push_back(vertex);
        vertex.position = { i - float(quadsPerTileSide), 0, 1 };
        vertices.push_back(vertex);
      }

      // Vertical vertices
      for (uint32_t i = 0; i < verticesPerTileSide * 2; i++)
      {
        vertex.position = { 0, 0, i - float(quadsPerTileSide) };
        vertices.push_back(vertex);
        vertex.position = { 1, 0, i - float(quadsPerTileSide) };
        vertices.push_back(vertex);
      }

      indices.reserve(quadsPerTileSide * 24 + 6);

      for (uint32_t i = 0; i < quadsPerTileSide * 2 + 1; i++)
      {
        uint32_t bl = i * 2 + 0;
        uint32_t br = i * 2 + 1;
        uint32_t tl = i * 2 + 2;
        uint32_t tr = i * 2 + 3;

        indices.push_back(br);
        indices.push_back(bl);
        indices.push_back(tr);
        indices.push_back(bl);
        indices.push_back(tl);
        indices.push_back(tr);
      }

      uint32_t startOfVertical = verticesPerTileSide * 4;

      for (uint32_t i = 0; i < quadsPerTileSide * 2 + 1; i++)
      {
        if (i == quadsPerTileSide)
        {
          continue;
        }

        uint32_t bl = i * 2 + 0;
        uint32_t br = i * 2 + 1;
        uint32_t tl = i * 2 + 2;
        uint32_t tr = i * 2 + 3;

        indices.push_back(startOfVertical + br);
        indices.push_back(startOfVertical + tr);
        indices.push_back(startOfVertical + bl);
        indices.push_back(startOfVertical + bl);
        indices.push_back(startOfVertical + tr);
        indices.push_back(startOfVertical + tl);
      }
    }
  };

  struct Seam : MeshPrimitive
  {
    Seam(uint32_t quadsPerTileSide)
    {
      uint32_t quadsPerLevelSide = quadsPerTileSide * 4 + 1;
      uint32_t verticesPerLevelSide = quadsPerLevelSide + 1;

      vertices.reserve(verticesPerLevelSide * 4);

      Vertex vertex;

      vertices.resize(verticesPerLevelSide * 4);

      for (uint32_t i = 0; i < verticesPerLevelSide; i++)
      {
        vertex.position = { i, 0, 0 };
        vertices[verticesPerLevelSide * 0 + i] = vertex;
        vertex.position = { verticesPerLevelSide, 0, i };
        vertices[verticesPerLevelSide * 1 + i] = vertex;
        vertex.position = { verticesPerLevelSide - i, 0, verticesPerLevelSide };
        vertices[verticesPerLevelSide * 2 + i] = vertex;
        vertex.position = { 0, 0, verticesPerLevelSide - i };
        vertices[verticesPerLevelSide * 3 + i] = vertex;
      }

      indices.reserve(verticesPerLevelSide * 6);

      for (uint32_t i = 0; i < verticesPerLevelSide * 4; i += 2)
      {
        indices.push_back(i + 1);
        indices.push_back(i);
        indices.push_back(i + 2);
      }

      indices.push_back(0);
    }
  };

  std::vector<std::shared_ptr<GPUMesh>> GeoClipmap::generate(uint32_t tileResolution)
  {
    Tile tile(tileResolution);
    Filler filler(tileResolution);
    Trim trim(tileResolution);
    Cross cross(tileResolution);
    Seam seam(tileResolution);
    
    std::shared_ptr<GPUMesh> tileMesh = std::make_shared<GPUMesh>(tile);
    std::shared_ptr<GPUMesh> fillerMesh = std::make_shared<GPUMesh>(filler);
    std::shared_ptr<GPUMesh> trimMesh = std::make_shared<GPUMesh>(trim);
    std::shared_ptr<GPUMesh> crossMesh = std::make_shared<GPUMesh>(cross);
    std::shared_ptr<GPUMesh> seamMesh = std::make_shared<GPUMesh>(seam);

    std::vector<std::shared_ptr<GPUMesh>> meshes =
    {
      tileMesh,
      fillerMesh,
      trimMesh,
      crossMesh,
      seamMesh
    };

    return meshes;
  }

}