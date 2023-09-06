#include "vertex_buffer.h"

#include<glad/glad.h>
#include<GLFW/glfw3.h>

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
  glGenBuffers(1, &rendererID);
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
  glDeleteBuffers(1, &rendererID);
}

void VertexBuffer::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}