#pragma once

#include "../../math/types.h"

namespace Lotus
{

  class ShaderProgram;

  /*
    Structure with a mesh GPU identifiers
  */
  struct IndirectRenderMesh
  {
    uint32_t count;
    uint32_t firstIndex;
    uint32_t baseVertex;
    uint32_t references;
  };

  /*
    Structure with a material GPU identifiers
  */
  struct IndirectRenderMaterial
  {
    uint32_t ID = 0;
    uint32_t references;
  };

  /*
    Structure with an object GPU identifiers and auxiliary data
  */
  struct IndirectRenderObject
  {
    uint32_t ID = 0;
    Handler<IndirectRenderMesh> mesh;
    Handler<IndirectRenderMaterial> material;
    Handler<ShaderProgram> shader;
    glm::mat4 model;
    bool unbatched = true;
  };

  /*
    Batch for objects with the same shader
  */
  struct ShaderBatch
  {
    Handler<ShaderProgram> shader;
		uint32_t first;
		uint32_t count;
  };

  /*
    Batch for objects with the same mesh and shader
  */
  struct DrawBatch
  {
    Handler<IndirectRenderMesh> mesh;
    Handler<ShaderProgram> shader;
    uint32_t prevInstanceCount;
    uint32_t instanceCount;
  };

  /*
    Batch for a single object
  */
  struct ObjectBatch
  {
    Handler<IndirectRenderObject> object;
    Handler<IndirectRenderMesh> mesh;
    Handler<ShaderProgram> shader;

    bool operator<(const ObjectBatch& other) const
    {
      if (shader.handle != other.shader.handle)
      {
        return shader.handle < other.shader.handle;
      }

      if (mesh.handle != other.mesh.handle)
      {
        return mesh.handle < other.mesh.handle;
      }
      
      return object.handle < other.object.handle;
    }
  };

}