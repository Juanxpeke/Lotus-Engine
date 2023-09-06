#pragma once

class IndexBuffer
{
private:
  GLuint rendererID;
  unsigned int count;
public:
  IndexBuffer(const unsigned int* data, unsigned int count);
  ~IndexBuffer();

  void bind() const;
  void unbind() const;
};