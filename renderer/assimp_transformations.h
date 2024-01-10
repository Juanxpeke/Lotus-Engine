#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/quaternion.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

inline glm::mat4 AssimpToGlmMatrix(const aiMatrix4x4& mat)
{
  glm::mat4 m;
  for (int y = 0; y < 4; y++)
  {
    for (int x = 0; x < 4; x++)
    {
      m[x][y] = mat[y][x];
    }
  }
  return m;
}

inline glm::vec3 AssimpToGlmVec3(const aiVector3D& vec)
{
  return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::fquat AssimpToGlmQuat(const aiQuaternion& quat)
{
  glm::fquat q;
  q.x = quat.x;
  q.y = quat.y;
  q.z = quat.z;
  q.w = quat.w;

  return q;
}