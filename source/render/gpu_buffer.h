#pragma once

#include <iostream>
#include <cstring>
#include <limits>
#include <algorithm>
#include <vector>
#include <set>
#include "../util/log.h"
#include "../util/opengl_entry.h"
#include "../math/types.h"
#include "../math/gpu_primitives.h"
#include "../render/mesh.h"

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

      LOTUS_LOG_INFO("[Buffer Log] Deleted buffer with ID {0}", ID);
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

    void resize(size_t size)
    {
      filledSize = size;

      if (allocatedSize < filledSize)
      {
        reallocate(filledSize);
      }
    };

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
  struct SingleElementGPUBuffer : GPUBuffer<T, CPUMapEnabled>
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
  struct MultiElementGPUBuffer : GPUBuffer<T, false>
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

      int i = 0;
      
      while (i < allocationBlocks.size())
      {
        AllocationBlock& block = allocationBlocks[i];

        if (block.first <= first && (block.first + block.size) >= first + size)
        {
          LOTUS_LOG_WARN("[Buffer Warning] Tried to remove block inside a free region, buffer ID {0}", this->ID);
          return;
        }

        if (block.first >= first && (block.first + block.size) <= first + size)
        {
          allocationBlocks[i] = allocationBlocks.back();
          allocationBlocks.pop_back();
          continue;
        }

        if (block.first < first && (block.first + block.size) < first + size)
        {
          previousAllocationBlockIndex = i;
        }
        else if(block.first > first && (block.first + block.size) > first + size)
        {
          nextAllocationBlockIndex = i;
        }

        i++;
      }

      int finalBlockIndex;

      if (previousAllocationBlockIndex != -1)
      {
        finalBlockIndex = previousAllocationBlockIndex;

        AllocationBlock& prevBlock = allocationBlocks[previousAllocationBlockIndex];

        prevBlock.size += first + size - (prevBlock.first + prevBlock.size);
      }
      else
      {
        allocationBlocks.emplace_back(first, size);
        finalBlockIndex = allocationBlocks.size() - 1;
      }

      if (nextAllocationBlockIndex != -1)
      {
        AllocationBlock& finalBlock = allocationBlocks[finalBlockIndex];
        AllocationBlock& nextBlock = allocationBlocks[nextAllocationBlockIndex];

        finalBlock.size = nextBlock.first + nextBlock.size - (finalBlock.first + finalBlock.size); 

        allocationBlocks[nextAllocationBlockIndex] = allocationBlocks.back();
        allocationBlocks.pop_back();
      }
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
  struct VertexBuffer : public MultiElementGPUBuffer<MeshVertex>
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
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, position));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, normal));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, uv));
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tangent));
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, bitangent));

      glBindVertexArray(0);
      glBindBuffer(bufferType, 0);
    }

    uint32_t vertexArray;
  };

  /*
    Buffer for meshes indices
  */
  struct IndexBuffer : public MultiElementGPUBuffer<unsigned int>
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
  struct DrawIndirectBuffer : public SingleElementGPUBuffer<DrawElementsIndirectCommand, true>
  {
    DrawIndirectBuffer()
    {
      bufferType = GL_DRAW_INDIRECT_BUFFER;
    }
  };

  /*
    Buffer for generic single-element storage
  */
  template <typename T>
  struct ShaderStorageBuffer : public SingleElementGPUBuffer<T, true>
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

  /*
    Buffer for generic uniform storage
  */
  template <typename T>
  struct UniformBuffer
  {
    UniformBuffer() : ID(0), allocated(false), bindingPoint(0)
    {
      glGenBuffers(1, &ID);
    }

    UniformBuffer(const UniformBuffer& other) = delete;

    ~UniformBuffer()
    {
      glDeleteBuffers(1, &ID);
    }

    UniformBuffer& operator=(const UniformBuffer& other) = delete;

    void setBindingPoint(uint32_t newBindingPoint)
    {
      glBindBufferBase(GL_UNIFORM_BUFFER, newBindingPoint, ID);
      bindingPoint = newBindingPoint;
    }

    void bind()
    {
      glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ID);
    }

    void unbind()
    {
      glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0);
    }

    void allocate()
    {
      if (allocated)
      {
        LOTUS_LOG_WARN("[Buffer Warning] Tried to allocate already allocated buffer with ID {0}", ID);
        return;
      }

      glBindBuffer(GL_UNIFORM_BUFFER, ID);
      glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_DYNAMIC_DRAW);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      allocated = true;

      LOTUS_LOG_INFO("[Buffer Log] Allocated buffer with ID {0}", ID);
    }

    void write(const T* source)
    {
      glBindBuffer(GL_UNIFORM_BUFFER, ID);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), source);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    uint32_t ID;
    bool allocated;

    uint32_t bindingPoint;
  };
}