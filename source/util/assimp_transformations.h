#pragma once

#include <assimp/quaternion.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include "../math/types.h"

inline glm::mat4 assimpToMatrix(const aiMatrix4x4& mat)
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

inline glm::vec3 assimpToVec3(const aiVector3D& vec)
{
  return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::fquat assimpToQuat(const aiQuaternion& quat)
{
  glm::fquat q;
  q.x = quat.x;
  q.y = quat.y;
  q.z = quat.z;
  q.w = quat.w;

  return q;
}