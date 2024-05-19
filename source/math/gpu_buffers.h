#pragma once

#include <cstdint>
#include <glad/glad.h>
#include "gpu_primitives.h"

namespace Lotus {

  template <typename T>
  struct GPUBuffer
  {
    GPUBuffer() : ID(0), size(0), allocatedSize(0), bufferType(GL_SHADER_STORAGE_BUFFER) {}

    void allocate(uint64_t initialAllocationSize)
    {
      glBindBuffer(bufferType, ID);
      glBufferData(bufferType, initialAllocationSize * sizeof(T), nullptr, GL_DYNAMIC_DRAW);
      glBindBuffer(bufferType, 0);

      allocatedSize = initialAllocationSize;
    }    

    uint32_t ID;
    uint64_t size;
    uint64_t allocatedSize;
    uint32_t bufferType;
  };

  struct VertexBuffer : public GPUBuffer<Vertex>
  {
    VertexBuffer() {
      bufferType = GL_ARRAY_BUFFER;
    }
  };

  struct IndexBuffer : public GPUBuffer<unsigned int>
  {
    IndexBuffer() {
      bufferType = GL_ELEMENT_ARRAY_BUFFER;
    }
  };

  struct DrawIndirectBuffer : public GPUBuffer<DrawElementsIndirectCommand>
  {
    DrawIndirectBuffer() {
      bufferType = GL_DRAW_INDIRECT_BUFFER;
    }
  };

  template <typename T>
  struct ShaderStorageBuffer : public GPUBuffer
  {
    ShaderStorageBuffer() {
      bufferType = GL_SHADER_STORAGE_BUFFER;
    }
  };

}