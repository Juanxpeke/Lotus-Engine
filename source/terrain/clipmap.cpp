#include "clipmap.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Lotus
{
  Clipmap::Clipmap(uint32_t clipmapLevels, uint32_t clipmapTileResolution) :
    levels(clipmapLevels),
    tileResolution(clipmapTileResolution),
    tile(tileResolution),
    tileMesh(tile.vertices, tile.indices),
    filler(tileResolution),
    fillerMesh(filler.vertices, filler.indices),
    trim(tileResolution),
    trimMesh(trim.vertices, trim.indices)
  {
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

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable( GL_POLYGON_OFFSET_LINE );
    glPolygonOffset( -1, -1 );
    
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::vec3 cameraPosition = camera.getLocalTranslation();

    glUseProgram(clipmapProgram.getProgramID());

    glUniformMatrix4fv(ViewBinding, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(ProjectionBinding, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

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

      glBindVertexArray(tileMesh.getVertexArrayID());

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

          glDrawElements(GL_TRIANGLES, tileMesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);
        }
      }


      // Draw filler
      glBindVertexArray(fillerMesh.getVertexArrayID());

      glUniform2fv(OffsetBinding, 1, glm::value_ptr(snappedPos));

      glDrawElements(GL_TRIANGLES, fillerMesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);

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

      glBindVertexArray(trimMesh.getVertexArrayID());

      glUniformMatrix4fv(ModelBinding, 1, GL_FALSE, glm::value_ptr(rotationModels[rotationIndex]));
      glUniform2fv(OffsetBinding, 1, glm::value_ptr(tileCentre));
      
      glDrawElements(GL_TRIANGLES, trimMesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
  }

}