#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include "gl_utils.h"
#include "path_manager.h"

GLFWwindow* window;

// =========
// Processes
// =========

bool startGL(int width, int height, char* title) 
{
  { // GLFW
    if (!glfwInit())
    {
      fprintf(stderr, "ERROR: could not start GLFW3\n");
      return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    
    if (!window)
    {
      fprintf( stderr, "ERROR: could not open window with GLFW3\n" );
      glfwTerminate();
      return false;
    }

    glfwMakeContextCurrent(window);
  }

  { // glad
    gladLoadGL();
  }

  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);

  printf("Renderer: %s\n", renderer);
  printf("OpenGL version %s\n", version);

  return true;
}

void stopGL() { glfwTerminate(); }

// ================
// Shader compiling
// ================

std::string readShaderFile(const std::filesystem::path& shaderPath)
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

GLuint compileShader(const std::string& shaderCode, unsigned int type)
{
  const char* shaderSource = shaderCode.c_str();
  
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shaderSource, 0);
  glCompileShader(shader);
  checkShaderErrors(shader);

  return shader;
}

GLuint createRenderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath)
{
  GLuint program = glCreateProgram();

  std::string vertexShaderCode = readShaderFile(vertexShaderPath);
  GLuint vertexShader = compileShader(vertexShaderCode, GL_VERTEX_SHADER); 
  glAttachShader(program, vertexShader);

  std::string fragmentShaderCode = readShaderFile(fragmentShaderPath);
  GLuint fragmentShader = compileShader(fragmentShaderCode, GL_FRAGMENT_SHADER); 
  glAttachShader(program, fragmentShader);

  glLinkProgram(program);
  checkProgramErrors(program);

  return program;
}

GLuint createComputeProgram(const std::filesystem::path& computeShaderPath)
{
  GLuint program = glCreateProgram();

  std::string computeShaderCode = readShaderFile(computeShaderPath);
  GLuint computeShader = compileShader(computeShaderCode, GL_COMPUTE_SHADER); 
  glAttachShader(program, computeShader);

  glLinkProgram(program);
  checkProgramErrors(program);

  return program;
}

bool checkShaderErrors(GLuint shader)
{
  GLint params = -1;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params)
  {
    fprintf(stderr, "ERROR: shader %u did not compile\n", shader);
    return false;
  }
  return true;
}

bool checkProgramErrors(GLuint program)
{
  GLint params = -1;
  glGetProgramiv(program, GL_LINK_STATUS, &params);
  if (GL_TRUE != params)
  {
    fprintf(stderr, "ERROR: program %u did not link\n", program);
    return false;
  }
  return true;
}

// ==========
// Quad stuff
// ==========

GLuint createQuadVAO()
{
  GLuint vao = 0, vbo = 0;
  float verts[] = { -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
  
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), verts, GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);

  GLintptr stride = 4 * sizeof(float);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
  glEnableVertexAttribArray(1);
  
  GLintptr offset = 2 * sizeof(float);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*) offset);

  return vao;
}