#include "clipmap.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Lotus
{

  Clipmap::Clipmap(uint32_t clipmapLevels, uint32_t clipmapTileResolution, float clipmapQuadSize) :
    levels(clipmapLevels),
    tileResolution(clipmapTileResolution),
    quadSize(clipmapQuadSize),
    tile(tileResolution, quadSize),
    tileMesh(tile.vertices, tile.indices),
    filler(tileResolution, quadSize),
    fillerMesh(filler.vertices, filler.indices)
  {
    clipmapProgram = ShaderProgram(shaderPath("terrain/clipmap.vert"), shaderPath("terrain/clipmap.frag"));
  }

  void Clipmap::render(const Camera& camera)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable( GL_POLYGON_OFFSET_LINE );
    glPolygonOffset( -1, -1 );
    
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::vec3 cameraPosition = camera.getLocalTranslation();

    glUseProgram(clipmapProgram.getProgramID());
    
    for (uint32_t level = 0; level < levels; level++)
    {
      float scale = static_cast<float>(1 << level);

      glm::vec2 snappedPos;
      snappedPos.x = std::floorf(cameraPosition.x / scale) * scale;
      snappedPos.y = std::floorf(cameraPosition.z / scale) * scale;

      glm::vec2 tileSize((tileResolution * quadSize) << level);
      glm::vec2 levelOrigin = snappedPos - glm::vec2((tileResolution * quadSize) << (level + 1));


      glUniform1fv(2, 1, &scale);

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

          glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(viewMatrix));
          glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
          glUniform2fv(3, 1, glm::value_ptr(tileOffset));

          glDrawElements(GL_TRIANGLES, tileMesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);
        }
      }

      glBindVertexArray(fillerMesh.getVertexArrayID());

      glUniform2fv(3, 1, glm::value_ptr(snappedPos));

      glDrawElements(GL_TRIANGLES, fillerMesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
  }

}