#include "shader_program.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <glad/glad.h>

ShaderProgram::ShaderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) noexcept
{
  programID = 0;

  // Read shader files to string
  std::string vertexShaderCode = readShaderFile(vertexShaderPath);
  std::string fragmentShaderCode = readShaderFile(fragmentShaderPath);

  // Replace constants
  preProcessShader(vertexShaderCode);
  preProcessShader(fragmentShaderCode);

  if (vertexShaderCode.length() == 0 || fragmentShaderCode.length() == 0) {
    return; // TODO: Exception throwing
  }
  // Compile both shaders
  unsigned int vertexShader = compileShader(vertexShaderCode, GL_VERTEX_SHADER);
  unsigned int fragmentShader = compileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
  
  if (vertexShader && fragmentShader)
  {
    programID = linkProgram(vertexShader, fragmentShader);
  }
}

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept : programID(program.programID)
{
  program.programID = 0;
}

ShaderProgram::~ShaderProgram()
{
  if (programID) glDeleteProgram(programID);
}

ShaderProgram&  ShaderProgram::operator=(ShaderProgram&& program) noexcept
{
  if (&program == this)
    return *this;

  if (programID)
    glDeleteProgram(programID);

  programID = program.programID;
  program.programID = 0;
  
  return *this;
}

std::string ShaderProgram::readShaderFile(const std::filesystem::path& shaderPath) const noexcept
{
  std::ifstream shaderFileStream(shaderPath);

  if (!shaderFileStream.good())
  {
    return std::string();
  }

  std::stringstream shaderStringStream;
  shaderStringStream << shaderFileStream.rdbuf();
  shaderFileStream.close();

  return shaderStringStream.str();
}

void ShaderProgram::preProcessShader(std::string& shaderCode) const noexcept
{
  struct ShaderConstant {
    std::string key;
    std::string value;
  };

  std::array<ShaderConstant,4> shaderConstants = 
  { // TODO: Define constants in renderer class
    {
    {"${MAX_DIRECTIONAL_LIGHTS}", std::to_string(2)},
    {"${MAX_SPOT_LIGHTS}", std::to_string(2)} ,
    {"${MAX_POINT_LIGHTS}", std::to_string(2)},
    {"${MAX_BONES}", std::to_string(2)}
    }
  };
  
  for (ShaderConstant& shaderConstant : shaderConstants)
  {
    size_t pos = 0;
    while ((pos = shaderCode.find(shaderConstant.key, pos)) != std::string::npos) 
    {
      shaderCode.replace(pos, shaderConstant.key.length(), shaderConstant.value);
      pos += shaderConstant.value.length();
    }
  }

}

unsigned int ShaderProgram::compileShader(const std::string& shaderCode, unsigned int type) const noexcept
{
  const char* shaderSource = shaderCode.c_str();
  
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &shaderSource, 0);
  glCompileShader(shader);

  // Error handling
  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE)
  {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    char* message = (char*) alloca(length * sizeof(char));
    glGetShaderInfoLog(shader, length, &length, message);
    
    // TODO: Implement error log system
    std::cout 
      << "Failed to compile "
      << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
      << "shader"
      << std::endl;
    std::cout << message << std::endl;

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

unsigned int ShaderProgram::linkProgram(unsigned int vertexShader, unsigned int fragmentShader) noexcept
{
  unsigned int program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  // Error handling
  GLint linked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE)
  {
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    char* message = (char*) alloca(length * sizeof(char));
    glGetProgramInfoLog(program, length, &length, message);

    // TODO: Implement error log system
    std::cout << "Failed to link program" << std::endl;
    std::cout << message << std::endl;

    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
  return program;
}

void ShaderProgram::bind()
{
  glUseProgram(programID);
}

void ShaderProgram::unbind()
{
  glUseProgram(0);
}