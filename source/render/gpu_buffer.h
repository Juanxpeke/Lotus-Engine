#pragma once

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <algorithm>
#include <vector>
#include <set>
#include <glad/glad.h>
#include "../util/log.h"
#include "../math/gpu_primitives.h"

namespace Lotus
{

  template <typename T, bool CPUMapEnabled = true>
  struct GPUBuffer
  {
    GPUBuffer() :
      ID(0),
      bufferType(GL_SHADER_STORAGE_BUFFER),
      filledSize(0),
      allocatedSize(0),
      allocated(false)
    {
      glGenBuffers(1, &ID);
      
      CPUBuffer = nullptr;
    }

    GPUBuffer(const GPUBuffer& other) = delete;

    ~GPUBuffer()
    {
      glDeleteBuffers(1, &ID);

      if constexpr(CPUMapEnabled)
      {
        delete[] CPUBuffer;
      }
    }

    GPUBuffer& operator=(const GPUBuffer& other) = delete;

    virtual void bind()
    {
      glBindBuffer(bufferType, ID);
    }

    virtual void unbind()
    {
      glBindBuffer(bufferType, 0);
    }

    virtual void link() {}

    void allocate(size_t initialAllocationSize)
    {
      if (allocated)
      {
        LOTUS_LOG_WARN("[Buffer Warning] Tried to allocate already allocated buffer with ID {0}", ID);
        return;
      }

      glBindBuffer(bufferType, ID);
      glBufferData(bufferType, initialAllocationSize * sizeof(T), nullptr, GL_DYNAMIC_DRAW);
      glBindBuffer(bufferType, 0);

      if constexpr(CPUMapEnabled)
      {
        CPUBuffer = new T[initialAllocationSize];
      }
      
      allocatedSize = initialAllocationSize;
      allocated = true;

      LOTUS_LOG_INFO("[Buffer Log] Allocated buffer with ID {0} (Size = {1})", ID, initialAllocationSize);
    }

    void allocate(size_t initialAllocationSize, const T* initialAllocationData)
    {
      if (allocated)
      {
        LOTUS_LOG_WARN("[Buffer Warning] Tried to allocate already allocated buffer with ID {0}", ID);
        return;
      }

      glBindBuffer(bufferType, ID);
      glBufferData(bufferType, initialAllocationSize * sizeof(T), initialAllocationData, GL_DYNAMIC_DRAW);
      glBindBuffer(bufferType, 0);

      if constexpr(CPUMapEnabled)
      {
        CPUBuffer = new T[initialAllocationSize];

        std::memcpy(CPUBuffer, initialAllocationData, initialAllocationSize * sizeof(T));
      }
      
      allocatedSize = initialAllocationSize;
      allocated = true;

      LOTUS_LOG_INFO("[Buffer Log] Allocated buffer with ID {0} (Size = {1})", ID, initialAllocationSize);
    }

    void reallocate(size_t size)
    {
      if (!allocated)
      {
        LOTUS_LOG_WARN("[Buffer Warning] Tried to reallocate non-allocated buffer with ID {0}", ID);
        return;
      }

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

      if constexpr(CPUMapEnabled)
      {
        T* newCPUBuffer = new T[newAllocationSize];
        std::memcpy(newCPUBuffer, CPUBuffer, allocatedSize * sizeof(T));

        delete[] CPUBuffer;

        CPUBuffer = newCPUBuffer;
      }

      LOTUS_LOG_INFO("[Buffer Log] Reallocated buffer with ID {0} (Old ID = {1}, Size = {2}, Old Size = {3})", newID, ID, newAllocationSize, allocatedSize);

      ID = newID;
      allocatedSize = newAllocationSize;

      link();
    }

    void write(const T* source, uint32_t first, size_t size)
    {
      glBindBuffer(bufferType, ID);
      glBufferSubData(bufferType, first * sizeof(T), size * sizeof(T), source);
      glBindBuffer(bufferType, 0);
    }

    T* map()
    {
      if constexpr(CPUMapEnabled)
      {
        if (allocatedSize < filledSize)
        {
          reallocate(filledSize);
        }

        return CPUBuffer;
      }
      else
      {
        return (T*) (glMapNamedBuffer(ID, GL_WRITE_ONLY));
      }
    }

    void unmap()
    {
      if constexpr(CPUMapEnabled)
      {
        write(CPUBuffer, 0, filledSize);
      }
      else
      {
        glUnmapNamedBuffer(ID);
      }
    }

    uint32_t ID;
    uint32_t bufferType;
    size_t filledSize;
    size_t allocatedSize;
    bool allocated;

    // TODO: Declare this variable at compile time (not possible with if constexpr)
    T* CPUBuffer;
  };

  template <typename T, bool CPUMapEnabled = true>
  struct UniformGPUBuffer : GPUBuffer<T, CPUMapEnabled>
  {
    uint32_t add(const T* source)
    {
      uint32_t first = this->filledSize;

      if (!allocationPlaces.empty())
      {
        first = *allocationPlaces.begin();
        allocationPlaces.erase(allocationPlaces.begin());
      }
      
      if (first == this->filledSize)
      {
        this->filledSize = this->filledSize + 1;
      }

      if (this->filledSize > this->allocatedSize)
      {
        this->reallocate(this->filledSize);
      }

      this->write(source, first, 1);

      if constexpr(CPUMapEnabled)
      {
        this->CPUBuffer[first] = *source;
      }

      return first;
    }

    void remove(uint32_t first)
    {
      if (first < this->filledSize)
      {
        allocationPlaces.insert(first);
      }
      else
      {
        LOTUS_LOG_WARN("[Buffer Warning] Tried to remove element outside buffer scope, buffer ID {0}", this->ID);
        return;
      }
    }

    std::set<uint32_t> allocationPlaces;
  };

  template <typename T>
  struct NonUniformGPUBuffer : GPUBuffer<T, false>
  {
    uint32_t add(const T* source, size_t size = 1)
    {
      uint32_t first = this->filledSize;

      for (int i = 0; i < allocationBlocks.size(); i++)
      {
        int sizeDifference = allocationBlocks[i].size - size;

        if (sizeDifference == 0)
        {
          first = allocationBlocks[i].first;
          allocationBlocks[i] = allocationBlocks.back();
          allocationBlocks.pop_back();
          break;
        }
        else if (sizeDifference > 0)
        {
          first = allocationBlocks[i].first;
          allocationBlocks[i].first += size;
          allocationBlocks[i].size = sizeDifference;
          break;
        }
      }

      if (first + size > this->allocatedSize)
      {
        this->reallocate(first + size);
      }

      if (first + size > this->filledSize)
      {
        this->filledSize = first + size;
      }

      this->write(source, first, size);

      return first;
    }

    void remove(uint32_t first, size_t size = 1)
    {
      int previousAllocationBlockIndex = -1;
      int nextAllocationBlockIndex = -1;

      for (int i = 0; i < allocationBlocks.size(); i++)
      {
        // TODO: Indentify previous and next allocation blocks
      }

      // TODO: If they exist, do the merge, if not, then allocationBlocks.emplace_back(first, size);
    }

    struct AllocationBlock
    {
      AllocationBlock(uint32_t initialFirst, size_t initialSize) :
        first(initialFirst),
        size(initialSize)
      {}

      uint32_t first;
      size_t size;
    };

    std::vector<AllocationBlock> allocationBlocks;
  };

  /*
    Buffer for meshes vertices
  */
  struct VertexBuffer : public NonUniformGPUBuffer<Vertex>
  {
    VertexBuffer() : vertexArray(0)
    {
      bufferType = GL_ARRAY_BUFFER;
    }

    void setVertexArray(uint32_t newVertexArray)
    {
      vertexArray = newVertexArray;
      link();
    }

    virtual void link() override
    {
      glBindVertexArray(vertexArray);
      glBindBuffer(bufferType, ID);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));

      glBindVertexArray(0);
      glBindBuffer(bufferType, 0);
    }

    uint32_t vertexArray;
  };

  /*
    Buffer for meshes indices
  */
  struct IndexBuffer : public NonUniformGPUBuffer<unsigned int>
  {
    IndexBuffer() : vertexArray(0)
    {
      bufferType = GL_ELEMENT_ARRAY_BUFFER;
    }

    void setVertexArray(uint32_t newVertexArray)
    {
      vertexArray = newVertexArray;
      link();
    }

    virtual void link() override
    {
      glBindVertexArray(vertexArray);
      glBindBuffer(bufferType, ID);
      glBindVertexArray(0);
      glBindBuffer(bufferType, 0);
    }

    uint32_t vertexArray;
  };

  /*
    Buffer for draw commands
  */
  struct DrawIndirectBuffer : public UniformGPUBuffer<DrawElementsIndirectCommand, true>
  {
    DrawIndirectBuffer()
    {
      bufferType = GL_DRAW_INDIRECT_BUFFER;
    }
  };

  /*
    Buffer for generic uniform storage
  */
  template <typename T>
  struct ShaderStorageBuffer : public UniformGPUBuffer<T, true>
  {
    ShaderStorageBuffer() : bindingPoint(0)
    {
      this->bufferType = GL_SHADER_STORAGE_BUFFER;
    }

    void setBindingPoint(uint32_t newBindingPoint)
    {
      glBindBufferBase(this->bufferType, newBindingPoint, this->ID);
      bindingPoint = newBindingPoint;
    }

    virtual void bind() override
    {
      glBindBufferBase(this->bufferType, bindingPoint, this->ID);
    }

    virtual void unbind() override
    {
      glBindBufferBase(this->bufferType, bindingPoint, 0);
    }

    uint32_t bindingPoint;
  };

}