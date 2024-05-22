#pragma once

#include "primitives.h"

namespace Lotus
{ 
  struct DrawElementsIndirectCommand
  {
    uint32_t count = 0;         // # of indices
    uint32_t instanceCount = 0; // # of instances
    uint32_t firstIndex = 0;    // Index of the first element in the EBO
    uint32_t baseVertex = 0;    // Base value added to indices used in the EBO to read from the VBO
    uint32_t baseInstance = 0;  // gl_BaseInstance shader value
    uint32_t padding0 = 0;      // Padding due to GLSL layout std140 16 bytes alignment rule
    uint32_t padding1 = 0;
    uint32_t padding2 = 0;
  };

  struct GPUObjectData
  {
    glm::mat4 model;              // 64
    uint64_t materialHandle = 0;  // 72
    uint64_t padding0 = 0;        // 80
  };

  struct GPUMaterialData
  {
    glm::vec3 vec3_0;   // 12
    int int_0;          // 16
    glm::vec3 vec3_1;   // 28
    int int_1;          // 32
    uint64_t uint64_0;  // 40
    uint64_t uint64_1;  // 48
    uint64_t uint64_2;  // 56
    uint64_t uint64_3;  // 64
  };
  
  struct GPUInstance
  {
    uint32_t objectID;
    uint32_t drawBatchID;
  };

  struct GPUDirectionalLightData
  {
    glm::vec3 colorIntensity; // 12
    float padding04;          // 16
    glm::vec3 direction;      // 28
    float padding08;          // 32
  };

  struct GPUPointLightData
  {
    glm::vec3 colorIntensity; // 12
    float padding04;          // 16
    glm::vec3 position;       // 28
    float radius;             // 32
  };

  struct GPUSpotLightData
  {
    glm::vec3 colorIntensity; // 12
    float radius;             // 16
    glm::vec3 position;       // 28
    float cosPenumbraAngle;   // 32
    glm::vec3 direction;      // 44
    float cosUmbraAngle;      // 48
  };
}