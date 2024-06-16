#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <set>

namespace Lotus
{
  enum class ShaderType
  {
    Vertex,
    Fragment,
    Geometry,
    Compute
  };

  class Shader
  {
  public:
    Shader(const std::filesystem::path& shaderPath, ShaderType shaderType);
    ~Shader();

    uint32_t getID() const noexcept { return ID; };
    const std::filesystem::path& getPath() const noexcept { return path; } 

  private:
    std::string readFileFromPath(const std::filesystem::path& filePath);
    std::string preProcess(std::string fileCode, const std::filesystem::path& filePath, std::set<std::filesystem::path>& fileHistory);
    void compile();

    std::filesystem::path path;
    std::string code;
    ShaderType type;
    uint32_t ID;
  };

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
    
    ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
    ShaderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) noexcept;
    ShaderProgram() : programID(0) {}
    ShaderProgram(const ShaderProgram& program) = delete;
    ShaderProgram(ShaderProgram&& program) noexcept;
    ~ShaderProgram();

    ShaderProgram& operator=(const ShaderProgram& program) = delete;
    ShaderProgram& operator=(ShaderProgram&& program) noexcept;

    void bind();
    void unbind();
    uint32_t getProgramID() const noexcept { return programID; }

  private:
    void linkProgram(const Shader& vertexShader, const Shader& fragmentShader);
    
    uint32_t programID;
  };
}