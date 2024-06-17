#include "clipmap.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

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

  Clipmap::Clipmap(uint32_t clipmapLevels, uint32_t clipmapTileResolution) :
    levels(clipmapLevels),
    tileResolution(clipmapTileResolution)
  {

    Tile tile(tileResolution);
    Filler filler(tileResolution);
    Trim trim(tileResolution);
    
    tileMesh = std::make_shared<GPUMesh>(tile.vertices, tile.indices);
    fillerMesh = std::make_shared<GPUMesh>(filler.vertices, filler.indices);
    trimMesh = std::make_shared<GPUMesh>(trim.vertices, trim.indices);
    
    Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();
    heightmapTexture = textureLoader.generatePerlinTexture(720, 720);

    clipmapProgram = ShaderProgram(shaderPath("terrain/clipmap.vert"), shaderPath("terrain/clipmap.frag"));

    rotationModels[0] = glm::mat4(1.0f);
    rotationModels[1] = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationModels[2] = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationModels[3] = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  }

  void Clipmap::render(const Camera& camera)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uint16_t quadSize = 1;

    /*
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable( GL_POLYGON_OFFSET_LINE );
    glPolygonOffset( -1, -1 );
    */
   
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::vec3 cameraPosition = camera.getLocalTranslation();

    glUseProgram(clipmapProgram.getProgramID());

    glUniformMatrix4fv(ViewBinding, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(ProjectionBinding, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniform1i(HeightmapTextureBinding, HeightmapTextureUnit);

    glBindTextureUnit(HeightmapTextureUnit, heightmapTexture->getID());

    for (uint32_t level = 0; level < levels; level++)
    {
      float scale = static_cast<float>(1 << level);

      glm::vec2 snappedPos;
      snappedPos.x = std::floorf(cameraPosition.x / scale) * scale;
      snappedPos.y = std::floorf(cameraPosition.z / scale) * scale;

      glm::vec2 tileSize((tileResolution * quadSize) << level);
      glm::vec2 levelOrigin = snappedPos - glm::vec2((tileResolution * quadSize) << (level + 1));

      glUniform1fv(LevelScaleBinding, 1, &scale);
      glUniformMatrix4fv(ModelBinding, 1, GL_FALSE, glm::value_ptr(rotationModels[0]));

      glBindVertexArray(tileMesh->getVertexArrayID());

      for (int x = 0; x < 4; x++)
      {
        for (int y = 0; y < 4; y++)
        {
          if (level != 0 && (x == 1 || x == 2) && (y == 1 || y == 2))
          {
            continue;
          }

          glm::vec2 fill = glm::vec2((x >= 2 ? quadSize : 0), (y >= 2 ? quadSize : 0)) * scale;
          glm::vec2 tileOffset = levelOrigin + glm::vec2(x, y) * tileSize + fill;


          glUniform2fv(OffsetBinding, 1, glm::value_ptr(tileOffset));

          glDrawElements(GL_TRIANGLES, tileMesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
        }
      }


      // Draw filler
      glBindVertexArray(fillerMesh->getVertexArrayID());

      glUniform2fv(OffsetBinding, 1, glm::value_ptr(snappedPos));

      glDrawElements(GL_TRIANGLES, fillerMesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);

      if (level == levels - 1)
      {
        continue;
      }

      float nextScale = scale * 2.0f;

      glm::vec2 nextSnappedPos;
      nextSnappedPos.x = std::floorf(cameraPosition.x / nextScale) * nextScale;
      nextSnappedPos.y = std::floorf(cameraPosition.z / nextScale) * nextScale;
      
      // Draw trim
      glm::vec2 tileCentre = snappedPos + glm::vec2(scale * 0.5f);
      glm::vec2 d = glm::vec2(cameraPosition.x, cameraPosition.z) - nextSnappedPos;

      uint32_t rotationIndex = 0;
      rotationIndex |= (d.x >= scale ? 0 : 2);
      rotationIndex |= (d.y >= scale ? 0 : 1);

      glBindVertexArray(trimMesh->getVertexArrayID());

      glUniformMatrix4fv(ModelBinding, 1, GL_FALSE, glm::value_ptr(rotationModels[rotationIndex]));
      glUniform2fv(OffsetBinding, 1, glm::value_ptr(tileCentre));
      
      glDrawElements(GL_TRIANGLES, trimMesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
  }

}