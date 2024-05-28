#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <algorithm>
#include <vector>
#include <set>
#include <glad/glad.h>
#include "gpu_primitives.h"

namespace Lotus
{

  template <typename T>
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
    }

    ~GPUBuffer()
    {
      glDeleteBuffers(1, &ID);
      delete[] CPUBuffer;
    }

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
        return; // TODO: Log error
      }

      glBindBuffer(bufferType, ID);
      glBufferData(bufferType, initialAllocationSize * sizeof(T), nullptr, GL_DYNAMIC_DRAW);
      glBindBuffer(bufferType, 0);

      CPUBuffer = new T[initialAllocationSize];

      allocatedSize = initialAllocationSize;
      allocated = true;
    }

    void reallocate(size_t size)
    {
      if (!allocated)
      {
        return; // TODO: Throw error
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

      T* newCPUBuffer = new T[newAllocationSize];
      std::memcpy(newCPUBuffer, CPUBuffer, allocatedSize * sizeof(T));

      delete[] CPUBuffer;

      CPUBuffer = newCPUBuffer;

      ID = newID;
      allocatedSize = newAllocationSize;

      link();
    }

    void write(const T* source, uint32_t first, size_t size)
    {
      glBindBuffer(bufferType, ID);
      glBufferSubData(bufferType, first * sizeof(T), size * sizeof(T), source);
      glBindBuffer(bufferType, 0);

      filledSize = size;
    }

    T* map()
    {
      if (allocatedSize < filledSize)
      {
        reallocate(filledSize);
      }

      return CPUBuffer;
    }

    void unmap()
    {
      write(CPUBuffer, 0, filledSize);
    }

    uint32_t ID;
    uint32_t bufferType;
    size_t filledSize;
    size_t allocatedSize;
    bool allocated;

    T* CPUBuffer;
  };

  template <typename T>
  struct UniformGPUBuffer : GPUBuffer<T>
  {
    uint32_t add(const T* source)
    {
      uint32_t first = this->filledSize;

      if (allocationPlaces.size() > 0)
      {
        first = *allocationPlaces.rbegin();
        allocationPlaces.erase(allocationPlaces.rbegin());
      }

      if (first + this->size > this->allocatedSize)
      {
        reallocate(first + this->size);
      }

      if (first + this->size > this->filledSize)
      {
        this->filledSize = first + this->size;
      }

      write(source, first, this->size);
      //std::memcpy(CPUBuffer + first, source, size * sizeof(T));

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
        return; // TODO: Throw error
      }
    }

    std::set<uint32_t> allocationPlaces;
  };

  template <typename T>
  struct NonUniformGPUBuffer : GPUBuffer<T>
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
      //std::memcpy(CPUBuffer + first, source, size * sizeof(T));

      return first;
    }

    void remove(uint32_t first, size_t size = 1)
    {
      int previousAllocationBlockIndex = -1;
      int nextAllocationBlockIndex = -1;

      for (int i = 0; i < allocationBlocks.size(); i++)
      {
        
      }

      allocationBlocks.emplace_back(first, size);
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
  struct DrawIndirectBuffer : public UniformGPUBuffer<DrawElementsIndirectCommand>
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
  struct ShaderStorageBuffer : public UniformGPUBuffer<T>
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