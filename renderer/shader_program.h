#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <glm/glm.hpp>

class ShaderProgram
{
public:
  static constexpr int MvpMatrixShaderLocation = 0;
  static constexpr int ModelMatrixShaderLocation = 1;
  static constexpr int ModelInverseTransposeMatrixShaderLocation = 2;
  static constexpr int DiffuseColorShaderLocation = 3;
  static constexpr int DiffuseTextureSamplerShaderLocation = 3;
  static constexpr int MaterialTintShaderLocation = 4;

  static constexpr int DiffuseTextureUnit = 0;
  
  ShaderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) noexcept;
  ~ShaderProgram();
  
  void bind();
  void unbind();
  uint32_t getProgramID() const noexcept { return programID; }

private:
  std::string readShaderFile(const std::filesystem::path& shaderPath) const noexcept;
  void preProcessShader(std::string& shaderCode) const noexcept;
  unsigned int compileShader(const std::string& shaderCode, unsigned int type) const noexcept;
  unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader) noexcept;
  
  uint32_t programID;
};