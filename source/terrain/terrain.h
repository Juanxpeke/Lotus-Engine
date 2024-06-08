#pragma once

#include <cstdint>
#include <glad/glad.h>
#include "../math/gpu_buffers.h"
#include "terrain_data.h"

namespace Lotus
{
  class Terrain
  {
  friend class Renderer;

  public:
    Terrain(unsigned int chunkSize) :
      terrainData(chunkSize, chunkSize)
    {
      glGenVertexArrays(1, &vertexArrayID);

      vertexBuffer.allocate(chunkSize * chunkSize);
      vertexBuffer.setVertexArray(vertexArrayID);

      indexBuffer.allocate((chunkSize - 1) * (chunkSize - 1) * 6);
      indexBuffer.setVertexArray(vertexArrayID);

      Vertex vertex;
      unsigned int indices[6];

      int vertexIndex = 0;

      for (int y = 0; y < chunkSize; y++)
      {
        for (int x = 0; x < chunkSize; x++)
        {
          
          vertex.position = { x, terrainData.get(x, y), y };
          vertex.uv = { x / chunkSize, y / chunkSize };

          vertexBuffer.add(&vertex, 1);

          if (x < chunkSize - 1 && y < chunkSize - 1)
          {
            indices[0] = vertexIndex;                 indices[1] = vertexIndex + chunkSize; indices[2] = vertexIndex + chunkSize + 1;
            indices[3] = vertexIndex + chunkSize + 1; indices[4] = vertexIndex + 1;         indices[5] = vertexIndex; 

            indexBuffer.add(indices, 6);
          }

          vertexIndex++;
        }
      }
    }

  private:
    TerrainData terrainData;

    uint32_t vertexArrayID;
    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
  };

}