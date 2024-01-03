#include "mesh.h"

#include <iostream>
#include <vector>
#include <stack>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

Mesh::Mesh(PrimitiveType type) :
  vertexArrayID(0),
  vertexBufferID(0),
  indexBufferID(0),
  indexBufferCount(0)
{
  switch (type)
  {
    case Mesh::PrimitiveType::Plane:
    {
      createPlane();
      break;
    }
    case Mesh::PrimitiveType::Cube:
    {
      createCube();
      break;
    }
    case Mesh::PrimitiveType::Sphere:
    {
      createSphere();
      break;
    }
    default:
    {
      createSphere();
      break;
    }
  }
}

Mesh::~Mesh() {
  if (vertexArrayID)
  { // TODO: Handle errors
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    vertexArrayID = 0;
  }
}

void Mesh::createPrimitive(std::vector<float>& vertices, std::vector<unsigned int>& indices) noexcept
{
  unsigned int VAO, VBO, EBO;
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  vertexArrayID = VAO;
  vertexBufferID = VBO;
  indexBufferID = EBO;
  indexBufferCount = static_cast<uint32_t>(indices.size());
}

void Mesh::createPlane() noexcept
{
  // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
  std::vector<float> planeVertices = 
  {
    -1.0f, -1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
  };

  std::vector<unsigned int> planeIndices =
  {
    0, 1, 2, 2, 3, 0
  };

  createPrimitive(planeVertices, planeIndices);
}

void Mesh::createCube() noexcept
{
  // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
  std::vector<float> cubeVertices =
  {
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,

    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f
  };

  std::vector<unsigned int> cubeIndices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
  };
  
  createPrimitive(cubeVertices, cubeIndices);
}

void Mesh::createSphere() noexcept
{
  // Source: http://www.songho.ca/opengl/gl_sphere.html
  // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
  std::vector<float> sphereVertices;
  std::vector<unsigned int> sphereIndices;

  unsigned int stackCount = 16;
  unsigned int sectorCount = 32;

  constexpr float PI = glm::pi<float>();
  
  float sectorStep = 2 * PI / sectorCount;
  float stackStep = PI / stackCount;
  float sectorAngle, stackAngle; // Phi and theta spherical cooordinates angles
  float radius = 1.0f;

  float x, y, z;
  for (unsigned int i = 0; i <= stackCount; i++)
  {
    stackAngle = PI / 2.0f - i * stackStep;
    float cosStackAngle = std::cos(stackAngle);
    float sinStackAngle = std::sin(stackAngle);
    z = sinStackAngle;

    for (unsigned int j = 0; j <= sectorCount; j++)
    {
      sectorAngle = sectorStep * j;
      float cosSectorAngle = std::cos(sectorAngle);
      float sinSectorAngle = std::sin(sectorAngle);
      x = cosStackAngle * cosSectorAngle;
      y = cosStackAngle * sinSectorAngle;

      // Position
      sphereVertices.push_back(radius * x);
      sphereVertices.push_back(radius * y);
      sphereVertices.push_back(radius * z);

      // Normal
      sphereVertices.push_back(x);
      sphereVertices.push_back(y);
      sphereVertices.push_back(z);

      // UV
      float u = (float) j / (float) sectorCount;
      float v = (float) i / (float) stackCount;
      sphereVertices.push_back(u);
      sphereVertices.push_back(v);
      
      // Tangent = dr / dSectorAngle
      float tx = -sinSectorAngle;
      float ty = cosSectorAngle;
      float tz = 0.0f;
      sphereVertices.push_back(tx);
      sphereVertices.push_back(ty);
      sphereVertices.push_back(tz);
      
      // Bitangent = dr / dStackAngle
      float bx = -sinStackAngle * cosSectorAngle;
      float by = -sinStackAngle * sinSectorAngle;
      float bz = cosStackAngle;
      sphereVertices.push_back(bx);
      sphereVertices.push_back(by);
      sphereVertices.push_back(bz);
    }

  }

  unsigned int k1, k2;
  for (unsigned int i = 0; i < stackCount; ++i)
  {
    k1 = i * (sectorCount + 1);
    k2 = k1 + sectorCount + 1;

    for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
    {
      if (i != 0)
      {
        sphereIndices.push_back(k1);
        sphereIndices.push_back(k2);
        sphereIndices.push_back(k1 + 1);
      }

      if (i != (stackCount - 1))
      {
        sphereIndices.push_back(k1 + 1);
        sphereIndices.push_back(k2);
        sphereIndices.push_back(k2 + 1);
      }
    }
  }

  createPrimitive(sphereVertices, sphereIndices);
}