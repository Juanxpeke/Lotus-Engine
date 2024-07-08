#include "gpu_mesh.h"

#include <glad/glad.h>
#include "../util/log.h"

namespace Lotus
{
  GPUMesh::GPUMesh(const Mesh& mesh) : GPUMesh(mesh.getVertices(), mesh.getIndices()) {}

  GPUMesh::GPUMesh(const std::vector<Lotus::MeshVertex>& vertices, const std::vector<unsigned int>& indices)
  {
    glGenVertexArrays(1, &vertexArrayID);

    glBindVertexArray(vertexArrayID);

    vertexBuffer.allocate(vertices.size(), vertices.data());
    indexBuffer.allocate(indices.size(), indices.data());

    vertexBuffer.setVertexArray(vertexArrayID);
    indexBuffer.setVertexArray(vertexArrayID);

    indicesCount = indices.size();

    LOTUS_LOG_INFO("[Mesh Log] Created GPU mesh with VAO ID {0}", vertexArrayID);
  }

  GPUMesh::~GPUMesh()
  {
    if (vertexArrayID)
    {
      LOTUS_LOG_INFO("[Mesh Log] Deleted GPU mesh with VAO ID {0}", vertexArrayID);

      glDeleteVertexArrays(1, &vertexArrayID);
      vertexArrayID = 0;
    }
    else
    {
      LOTUS_LOG_ERROR("[Mesh Error] Tried to delete already deleted mesh");
    }
  }

}