#include "tra_mesh_gpu_link.h"

#include <glad/glad.h>
#include "../../math/primitives.h"

TraditionalMeshGPULink::TraditionalMeshGPULink(std::shared_ptr<Mesh> mesh)
{
  const std::vector<LotusMath::Vertex> vertices = mesh->getVertices();
  const std::vector<unsigned int> indices = mesh->getIndices();

  unsigned int VAO, VBO, EBO;
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LotusMath::Vertex), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, uv));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, tangent));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, bitangent));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  vertexArrayID = VAO;
  vertexBufferID = VBO;
  indexBufferID = EBO;
}

TraditionalMeshGPULink::~TraditionalMeshGPULink()
{
  if (vertexArrayID)
  { // TODO: Handle errors
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    vertexArrayID = 0;
  }
}