#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <glm/glm.hpp>

class ShaderProgram {
public:
  
  ShaderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) noexcept;

  void bind();

  void unbind();

  uint32_t getProgramID() const noexcept { return m_programID; }

  ~ShaderProgram();

private:
  std::string readShaderFile(const std::filesystem::path& shaderPath) const noexcept;
  unsigned int compileShader(const std::string& shaderCode, unsigned int type) const noexcept;
  unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader) noexcept;
  
  uint32_t m_programID;
};