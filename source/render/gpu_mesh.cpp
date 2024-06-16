#include "gpu_mesh.h"

#include <glad/glad.h>

namespace Lotus
{

  GPUMesh::GPUMesh(const std::vector<Lotus::Vertex>& vertices, const std::vector<unsigned int>& indices)
  {
    glGenVertexArrays(1, &vertexArrayID);
    glGenBuffers(1, &vertexBufferID);
    glGenBuffers(1, &indexBufferID);

    glBindVertexArray(vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Lotus::Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Lotus::Vertex), (void*) offsetof(Lotus::Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Lotus::Vertex), (void*) offsetof(Lotus::Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Lotus::Vertex), (void*) offsetof(Lotus::Vertex, uv));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Lotus::Vertex), (void*) offsetof(Lotus::Vertex, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Lotus::Vertex), (void*) offsetof(Lotus::Vertex, bitangent));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    indicesCount = indices.size();
  }

  GPUMesh::~GPUMesh()
  {
    if (vertexArrayID)
    { // TODO: Handle errors
      
      return;
      glDeleteBuffers(1, &vertexBufferID);
      glDeleteBuffers(1, &indexBufferID);
      glDeleteVertexArrays(1, &vertexArrayID);
      vertexArrayID = 0;
    }
  }

}