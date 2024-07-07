#include "terrain_renderer.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "../util/log.h"
#include "../render/identifiers.h"
#include "geoclipmap.h"

namespace Lotus
{

  TerrainRenderer::TerrainRenderer(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator, uint32_t terrainLevels, uint32_t terrainTileResolution) :
    levels(terrainLevels),
    tileResolution(terrainTileResolution),
    dataGenerator(terrainDataGenerator)
  {
    meshes = GeoClipmap::generate(tileResolution);

    Lotus::TextureConfig textureConfig;
    textureConfig.format = Lotus::TextureFormat::RFloat;
    textureConfig.width = dataGenerator->getDataPerChunkSide();
    textureConfig.height = dataGenerator->getDataPerChunkSide();
    textureConfig.depth = dataGenerator->getChunksAmount();

    heightmapTextures = std::make_shared<GPUArrayTexture>(textureConfig);
    updateHeightmapTextures(true);
    
    for (int x = 0; x < dataGenerator->getChunksPerSide(); x++)
    {
      for (int y = 0; y < dataGenerator->getChunksPerSide(); y++)
      {
        uint16_t layer = y * dataGenerator->getChunksPerSide() + x;
        heightmapTextures->setLayerData(layer, dataGenerator->getChunkData(x, y));
      }
    }

    clipmapProgram = ShaderProgram(shaderPath("terrain/clipmap.vert"), shaderPath("terrain/clipmap.frag"));

    proceduralBuffer.allocate();
    proceduralBuffer.setBindingPoint(ProceduralBufferBindingPoint);

    rotationModels[0] = glm::mat4(1.0f);
    rotationModels[1] = glm::rotate(glm::mat4(1.0f), glm::radians( 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationModels[2] = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationModels[3] = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    debugColors[0] = glm::vec3(1.0, 1.0, 1.0);
    debugColors[1] = glm::vec3(0.0, 1.0, 1.0);
    debugColors[2] = glm::vec3(0.0, 1.0, 0.0);
    debugColors[3] = glm::vec3(0.0, 0.0, 1.0);
    debugColors[4] = glm::vec3(1.0, 0.0, 0.0);
  }

  void TerrainRenderer::setDataGenerator(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator)
  {
    dataGenerator = terrainDataGenerator;
    updateHeightmapTextures(true);
  }

  void TerrainRenderer::render(const Camera& camera)
  {
    glm::vec3 cameraPosition = camera.getLocalTranslation();

    glUseProgram(clipmapProgram.getProgramID());



    glUniform1i(HeightmapTextureArrayBinding, HeightmapTextureUnit);

    glBindTextureUnit(HeightmapTextureUnit, heightmapTextures->getID());

    updateHeightmapTextures();

    fillProceduralBuffer();

    proceduralBuffer.bind();

    // Draw cross
    {
      float scale = 1.0;

      glm::vec2 snappedPos;
      snappedPos.x = std::floorf(cameraPosition.x);
      snappedPos.y = std::floorf(cameraPosition.z);

      glUniform1fv(LevelScaleBinding, 1, &scale);
      glUniformMatrix4fv(ModelBinding, 1, GL_FALSE, glm::value_ptr(rotationModels[0]));
      glUniform2fv(OffsetBinding, 1, glm::value_ptr(snappedPos));
      glUniform3fv(DebugColorBinding, 1, glm::value_ptr(debugColors[GeoClipmap::CROSS]));
      
      glBindVertexArray(meshes[GeoClipmap::CROSS]->getVertexArrayID());
      
      glDrawElements(GL_TRIANGLES, meshes[GeoClipmap::CROSS]->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

    for (uint32_t level = 0; level < levels; level++)
    {
      float scale = static_cast<float>(1 << level);


      
      glm::vec2 snappedPos;
      snappedPos.x = std::floorf(cameraPosition.x / scale) * scale;
      snappedPos.y = std::floorf(cameraPosition.z / scale) * scale;

      glm::vec2 tileSize(tileResolution << level);
      glm::vec2 levelOrigin = snappedPos - glm::vec2(tileResolution << (level + 1));

      glUniform1fv(LevelScaleBinding, 1, &scale);
      glUniformMatrix4fv(ModelBinding, 1, GL_FALSE, glm::value_ptr(rotationModels[0]));

      glBindVertexArray(meshes[GeoClipmap::TILE]->getVertexArrayID());

      for (int x = 0; x < 4; x++)
      {
        for (int y = 0; y < 4; y++)
        {
          if (level != 0 && (x == 1 || x == 2) && (y == 1 || y == 2))
          {
            continue;
          }

          glm::vec2 fill = glm::vec2((x >= 2 ? 1 : 0), (y >= 2 ? 1 : 0)) * scale;
          glm::vec2 tileOffset = levelOrigin + glm::vec2(x, y) * tileSize + fill;


          glUniform2fv(OffsetBinding, 1, glm::value_ptr(tileOffset));
          glUniform3fv(DebugColorBinding, 1, glm::value_ptr(debugColors[GeoClipmap::TILE]));

          glDrawElements(GL_TRIANGLES, meshes[GeoClipmap::TILE]->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
        }
      }

      // Draw filler
      {
        glBindVertexArray(meshes[GeoClipmap::FILLER]->getVertexArrayID());

        glUniform2fv(OffsetBinding, 1, glm::value_ptr(snappedPos));
        glUniform3fv(DebugColorBinding, 1, glm::value_ptr(debugColors[GeoClipmap::FILLER]));

        glDrawElements(GL_TRIANGLES, meshes[GeoClipmap::FILLER]->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
      }

      if (level == levels - 1)
      {
        continue;
      }

      float nextScale = scale * 2.0f;

      glm::vec2 nextSnappedPos;
      nextSnappedPos.x = std::floorf(cameraPosition.x / nextScale) * nextScale;
      nextSnappedPos.y = std::floorf(cameraPosition.z / nextScale) * nextScale;
      
      // Draw trim
      {
        glUniform1fv(LevelScaleBinding, 1, &scale);

        glm::vec2 tileCentre = snappedPos + glm::vec2(int(scale) >> 1);
        glm::vec2 d = glm::vec2(cameraPosition.x, cameraPosition.z) - nextSnappedPos;

        uint32_t rotationIndex = 0;
        rotationIndex |= (d.x >= scale ? 0 : 2);
        rotationIndex |= (d.y >= scale ? 0 : 1);

        glBindVertexArray(meshes[GeoClipmap::TRIM]->getVertexArrayID());

        glUniformMatrix4fv(ModelBinding, 1, GL_FALSE, glm::value_ptr(rotationModels[rotationIndex]));
        glUniform2fv(OffsetBinding, 1, glm::value_ptr(tileCentre));
        glUniform3fv(DebugColorBinding, 1, glm::value_ptr(debugColors[GeoClipmap::TRIM]));
        
        glDrawElements(GL_TRIANGLES, meshes[GeoClipmap::TRIM]->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
      }
      // Draw seam
      {
        glm::vec2 nextBase = nextSnappedPos - glm::vec2(static_cast<float>(tileResolution << (level + 1)));

        glBindVertexArray(meshes[GeoClipmap::SEAM]->getVertexArrayID());

        glUniformMatrix4fv(ModelBinding, 1, GL_FALSE, glm::value_ptr(rotationModels[0]));
        glUniform2fv(OffsetBinding, 1, glm::value_ptr(nextBase));
        glUniform3fv(DebugColorBinding, 1, glm::value_ptr(debugColors[GeoClipmap::SEAM]));

        glDrawElements(GL_TRIANGLES, meshes[GeoClipmap::SEAM]->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
      }
    }
    
    glBindVertexArray(0);
  }

  void TerrainRenderer::fillProceduralBuffer()
  {
    ProceduralData proceduralData;

    proceduralData.dataPerChunkSide = dataGenerator->getDataPerChunkSide();
    proceduralData.chunksPerSide = dataGenerator->getChunksPerSide();
    proceduralData.dataOrigin = dataGenerator->getDataOrigin();
    proceduralData.chunksOrigin = dataGenerator->getChunksOrigin();

    proceduralBuffer.write(&proceduralData);
  }

  void TerrainRenderer::updateHeightmapTextures(bool forced)
  {
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::Everything) || forced)
    {
      for (int x = 0; x < dataGenerator->getChunksPerSide(); x++)
      {
        for (int y = 0; y < dataGenerator->getChunksPerSide(); y++)
        {
          uint16_t layer = y * dataGenerator->getChunksPerSide() + x;
          heightmapTextures->setLayerData(layer, dataGenerator->getChunkData(x, y));
        }
      }
    }

    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::TopChunks))
    {
      for (int x = 0; x < dataGenerator->getChunksPerSide(); x++)
      {
        uint16_t layer = dataGenerator->getChunksTop() * dataGenerator->getChunksPerSide() + x;
        heightmapTextures->setLayerData(layer, dataGenerator->getChunkData(x, dataGenerator->getChunksTop()));
      }
    }
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::RightChunks))
    {
      for (int y = 0; y < dataGenerator->getChunksPerSide(); y++)
      {
        uint16_t layer = y * dataGenerator->getChunksPerSide() + dataGenerator->getChunksRight();
        heightmapTextures->setLayerData(layer, dataGenerator->getChunkData(dataGenerator->getChunksRight(), y));
      }
    }
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::BottomChunks))
    {
      for (int x = 0; x < dataGenerator->getChunksPerSide(); x++)
      {
        uint16_t layer = dataGenerator->getChunksBottom() * dataGenerator->getChunksPerSide() + x;
        heightmapTextures->setLayerData(layer, dataGenerator->getChunkData(x, dataGenerator->getChunksBottom()));
      }
    }
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::LeftChunks))
    {
      for (int y = 0; y < dataGenerator->getChunksPerSide(); y++)
      {
        uint16_t layer = y * dataGenerator->getChunksPerSide() + dataGenerator->getChunksLeft();
        heightmapTextures->setLayerData(layer, dataGenerator->getChunkData(dataGenerator->getChunksLeft(), y));
      }
    }
  }
}