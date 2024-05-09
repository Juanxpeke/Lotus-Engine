#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace LotusMath
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
  };

  struct MeshPrimitive
  {
    std::vector<Vertex> vertices = {};
    std::vector<unsigned int> indices = {};

    MeshPrimitive() = default;
    MeshPrimitive(const MeshPrimitive& mesh): vertices(mesh.vertices), indices(mesh.indices) {};
    MeshPrimitive(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices): vertices(vertices), indices(indices) {};
  };

  struct Plane : MeshPrimitive
  {
    Plane()
    {
      // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
      vertices = 
      {
        { { -1.0f, -1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { {  1.0f, -1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { {  1.0f,  1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { { -1.0f,  1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } }
      };

      indices =
      {
        0, 1, 2, 2, 3, 0
      };
    }

  };

  struct Cube : MeshPrimitive
  {
    Cube()
    {
      // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
      vertices =
      {
        { { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
        { {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
        { {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
        { { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },

        { { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },
        { {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },
        { {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },
        { { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },

        { { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },
        { { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },
        { { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },
        { { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },

        { {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },
        { {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },
        { {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },
        { {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },

        { { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
        { {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
        { {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
        { { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },

        { { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
        { {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
        { {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
        { { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } }
      };

      indices =
      {
         0,  1,  2,  2,  3,  0,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
      };
    }
  };

  struct Sphere : MeshPrimitive
  {
    Sphere()
    {
      // Source: http://www.songho.ca/opengl/gl_sphere.html
      // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
      unsigned int stackCount = 16;
      unsigned int sectorCount = 32;

      constexpr float PI = glm::pi<float>();

      float sectorStep = 2 * PI / sectorCount;
      float stackStep = PI / stackCount;
      float sectorAngle, stackAngle; // Phi and theta spherical coordinates angles
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

          Vertex vertex;

          vertex.position = { radius * x, radius * y, radius * z};
          vertex.normal = { x, y, z };
          vertex.uv = { (float) j / (float) sectorCount, (float) i / (float) stackCount };
          vertex.tangent = { -sinSectorAngle, cosSectorAngle, 0.0f };
          vertex.bitangent = { -sinStackAngle * cosSectorAngle, -sinStackAngle * sinSectorAngle, cosStackAngle };

          vertices.push_back(vertex);
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
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);
          }

          if (i != (stackCount - 1))
          {
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
          }
        }
      }
    }
  };
}