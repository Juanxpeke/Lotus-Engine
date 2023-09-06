#pragma once

class VertexBuffer
{
private:
  GLuint rendererID;
public:
  VertexBuffer(const void* data, unsigned int size);
  ~VertexBuffer();

  void bind() const;
  void unbind() const;
};