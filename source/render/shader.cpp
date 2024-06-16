#include "shader.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <glad/glad.h>
#include "../util/log.h"
// #include "renderer.h"

namespace Lotus
{
  GLenum shaderTypeEnumToOpenGLEnum(ShaderType shaderType)
  {
    switch(shaderType)
    {
      case ShaderType::Vertex:
        return GL_VERTEX_SHADER;
      case ShaderType::Fragment:
        return GL_FRAGMENT_SHADER;
      case ShaderType::Compute:
        return GL_COMPUTE_SHADER;
      case ShaderType::Geometry:
        return GL_GEOMETRY_SHADER;
      default:
        return GL_COMPUTE_SHADER;
    }
     
    return GL_INVALID_ENUM;
  }

  std::string shaderTypeEnumToString(ShaderType shaderType)
  {
    switch(shaderType)
    {
      case ShaderType::Vertex:
        return "vertex";
      case ShaderType::Fragment:
        return "fragment";
      case ShaderType::Compute:
        return "compute";
      case ShaderType::Geometry:
        return "geometry";
      default:
        return "compute";
    }
     
    return "invalid";
  }

  Shader::Shader(const std::filesystem::path& shaderPath, ShaderType shaderType) :
    path(shaderPath),
    type(shaderType)
  {
    ID = 0;

    std::set<std::filesystem::path> includeFileHistory;
    includeFileHistory.insert(path);

    // Read shader file to string
    code = readFileFromPath(path);
    code = preProcess(code, path, includeFileHistory);

    compile();
  }

  Shader::~Shader()
  {
    if (ID)
    {
      glDeleteShader(ID);
    }
  }

  std::string Shader::readFileFromPath(const std::filesystem::path& filePath)
  {
    std::ifstream fileStream(filePath);

    if (!fileStream.good())
    {
      LOTUS_LOG_ERROR("[Shader Error] Couldn't open file at {0}", filePath.string());
      LOTUS_ASSERT(false, "Exiting");
      
      return std::string();
    }

		std::stringstream fileStringStream;
		fileStringStream << fileStream.rdbuf();
		fileStream.close();
		return fileStringStream.str();
  }

  std::string Shader::preProcess(std::string fileCode, const std::filesystem::path& filePath, std::set<std::filesystem::path>& fileHistory)
  {
    std::string filePathString = filePath.string();
    std::string directory = filePathString.substr(0, filePathString.find_last_of("/\\"));
    
    std::string preProcessedCode, line;

    std::istringstream fileStream(fileCode);

    while (std::getline(fileStream, line))
    {
      if (line.substr(0, 8) == "#include")
      {
        std::filesystem::path includePath = directory + "/" + line.substr(9);
        std::string includeRawCode = readFileFromPath(includePath);

        for (const std::filesystem::path& file : fileHistory)
        {
          if (std::filesystem::equivalent(file, includePath))
          {
            LOTUS_LOG_ERROR("[Shader Error] Cyclic include with file at {0}", includePath.string());
            LOTUS_ASSERT(false, "Exiting");
            return std::string();
          }
        }

        fileHistory.insert(includePath);

        
        std::string includeCode = preProcess(includeRawCode, includePath, fileHistory);

        preProcessedCode += includeCode;
      }
      else
      {
        preProcessedCode += line + "\n";
      }
    }

    struct ShaderConstant
    {
      std::string key;
      std::string value;
    };

    std::array<ShaderConstant, 3> shaderConstants = 
    {
      {
      {"${MAX_DIRECTIONAL_LIGHTS}", std::to_string(2)}, //Renderer::HalfMaxDirectionalLights * 2)},
      {"${MAX_POINT_LIGHTS}", std::to_string(2)}, //Renderer::HalfMaxPointLights * 2)},
      {"${MAX_SPOT_LIGHTS}", std::to_string(2)}, //Renderer::HalfMaxSpotLights * 2)}
      }
    };
    
    for (ShaderConstant& shaderConstant : shaderConstants)
    {
      size_t pos = 0;
      while ((pos = preProcessedCode.find(shaderConstant.key, pos)) != std::string::npos) 
      {
        preProcessedCode.replace(pos, shaderConstant.key.length(), shaderConstant.value);
        pos += shaderConstant.value.length();
      }
    }

    return preProcessedCode;
  }

  void Shader::compile()
  {
    const char* shaderSource = code.c_str();
    
    unsigned int shader = glCreateShader(shaderTypeEnumToOpenGLEnum(type));
    glShaderSource(shader, 1, &shaderSource, 0);
    glCompileShader(shader);

    // Error handling
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE)
    {
      GLint length = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

      char* message = new char[length];
      glGetShaderInfoLog(shader, length, &length, message);
      
      glDeleteShader(shader);

      std::string typeString = shaderTypeEnumToString(type);
      std::string messageString = message;
      delete[] message;

      LOTUS_LOG_ERROR("[Shader Error] Failed to compile {0} shader at {1}", typeString, path.string());
      LOTUS_LOG_ERROR("[Shader Error] GLSL error message\n\n{0}", messageString);
      LOTUS_ASSERT(false, "Exiting");
    }

    ID = shader;
  }

  ShaderProgram::ShaderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) noexcept
  {
    programID = 0;

    Shader vertexShader(vertexShaderPath, ShaderType::Vertex);
    Shader fragmentShader(fragmentShaderPath, ShaderType::Fragment);
    
    linkProgram(vertexShader, fragmentShader);
  }

  ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept
  {
    programID = program.programID;
    program.programID = 0;
  }

  ShaderProgram::~ShaderProgram()
  {
    if (programID)
    {
      glDeleteProgram(programID);
    }
  }

  ShaderProgram&  ShaderProgram::operator=(ShaderProgram&& program) noexcept
  {
    if (&program == this)
    {
      return *this;
    }

    if (programID)
    {
      glDeleteProgram(programID);
    }

    programID = program.programID;
    program.programID = 0;
    
    return *this;
  }

  void ShaderProgram::linkProgram(const Shader& vertexShader, const Shader& fragmentShader)
  {
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader.getID());
    glAttachShader(program, fragmentShader.getID());
    glLinkProgram(program);

    // Error handling
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (linked == GL_FALSE)
    {
      GLint length = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

      char* message = new char[length];
      glGetProgramInfoLog(program, length, &length, message);

      glDeleteProgram(program);
      glDeleteShader(vertexShader.getID());
      glDeleteShader(fragmentShader.getID());

      std::string messageString = message;
      delete[] message;

      LOTUS_LOG_ERROR("[Shader Error] Failed to link vertex and fragment shaders");
      LOTUS_LOG_ERROR("[Shader Error] Vertex shader file at {0}", vertexShader.getPath().string());
      LOTUS_LOG_ERROR("[Shader Error] Fragment shader file at {0}", fragmentShader.getPath().string());
      LOTUS_LOG_ERROR("[Shader Error] GLSL error message\n\n{0}", messageString);
      LOTUS_ASSERT(false, "Exiting");
    }
    
    programID = program;
  }

  void ShaderProgram::bind()
  {
    glUseProgram(programID);
  }

  void ShaderProgram::unbind()
  {
    glUseProgram(0);
  }
}
