#pragma once

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <algorithm>
#include <glad/glad.h>
#include "gpu_primitives.h"

namespace Lotus {

  template <typename T>
  struct GPUBuffer
  {
    GPUBuffer() :
      ID(0),
      bufferType(GL_SHADER_STORAGE_BUFFER),
      currentSize(0),
      allocatedSize(0),
      allocated(false)
    {
      glGenBuffers(1, &ID);
    }

    ~GPUBuffer()
    {
      glDeleteBuffers(1, &ID);
      delete[] CPUBuffer;
    }

    void allocate(size_t initialAllocationSize)
    {
      glBindBuffer(bufferType, ID);
      glBufferData(bufferType, initialAllocationSize * sizeof(T), nullptr, GL_DYNAMIC_DRAW);
      glBindBuffer(bufferType, 0);

      CPUBuffer = new T[initialAllocationSize];

      allocatedSize = initialAllocationSize;
      allocated = true;
    }

    void reallocate(size_t size)
    {
      if (allocated)
      {
        std::cout << "Reallocating" << std::endl;

        size_t newAllocationSize = allocatedSize;

        while (size > newAllocationSize && newAllocationSize < std::numeric_limits<size_t>::max() / 2)
        {
          newAllocationSize *= 2;
        }

        uint32_t newID;

        glGenBuffers(1, &newID);
        glBindBuffer(bufferType, newID);
        glBufferData(bufferType, newAllocationSize * sizeof(T), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, ID);

        glCopyBufferSubData(GL_COPY_READ_BUFFER, bufferType, 0, 0, std::min(allocatedSize, newAllocationSize) * sizeof(T));

        glBindBuffer(bufferType, 0);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);

        glDeleteBuffers(1, &ID);

        T* newCPUBuffer = new T[newAllocationSize];
        std::memcpy(newCPUBuffer, CPUBuffer, allocatedSize * sizeof(T));

        delete[] CPUBuffer;

        CPUBuffer = newCPUBuffer;

        ID = newID;
        allocatedSize = newAllocationSize;

        reset();
      }
      else
      {
        // TODO: Log error
      }
    }

    void write(T* source, uint32_t size, uint32_t first = 0)
    {
      if (first + size >= allocatedSize)
      {
        reallocate(first + size);
      }

      glBindBuffer(bufferType, ID);
      glBufferSubData(bufferType, first * sizeof(T), size * sizeof(T), source);
      glBindBuffer(bufferType, 0);

      currentSize = size;
    }

    T* map()
    {
      if (allocatedSize < currentSize)
      {
        reallocate(currentSize);
      }

      return CPUBuffer;
      // return (T*)(glMapNamedBuffer(ID, GL_WRITE_ONLY));
    }

    void unmap()
    {
      write(CPUBuffer, currentSize);
      // glUnmapNamedBuffer(ID);
    }

    virtual void reset() {}

    uint32_t ID;
    uint32_t bufferType;
    size_t currentSize;
    size_t allocatedSize;
    bool allocated;

    T* CPUBuffer;
  };

  struct VertexBuffer : public GPUBuffer<Vertex>
  {
    VertexBuffer()
    {
      bufferType = GL_ARRAY_BUFFER;
    }
  };

  struct IndexBuffer : public GPUBuffer<unsigned int>
  {
    IndexBuffer()
    {
      bufferType = GL_ELEMENT_ARRAY_BUFFER;
    }
  };

  struct DrawIndirectBuffer : public GPUBuffer<DrawElementsIndirectCommand>
  {
    DrawIndirectBuffer()
    {
      bufferType = GL_DRAW_INDIRECT_BUFFER;
    }
  };

  template <typename T>
  struct ShaderStorageBuffer : public GPUBuffer<T>
  {
    ShaderStorageBuffer() : bindingPoint(0)
    {
      std::cout << "SSBO cons" << std::endl;
      this->bufferType = GL_SHADER_STORAGE_BUFFER;
    }

    void setBindingPoint(uint32_t newBindingPoint)
    {
      std::cout << "SSBO BP" << std::endl;
      glBindBufferBase(this->bufferType, newBindingPoint, this->ID);
      bindingPoint = newBindingPoint;
    }

    virtual void reset() override
    {
      glBindBufferBase(this->bufferType, bindingPoint, this->ID);
    }

    uint32_t bindingPoint;
  };

}