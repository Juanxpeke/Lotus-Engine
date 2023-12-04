#include <iostream>
#include <sstream>
#include <fstream>
#include <format>
#include <map>
#include <vector>
#include "gl_utils.h"
#include "path_manager.h"

GLFWwindow* window;
std::string windowTitle;

const std::vector<Vertex2D_Flat> triangleVerticesFlat =
{ 
  // XY
  { -0.5f, -0.5f },
  {  0.5f, -0.5f },
  {  0.0f,  0.5f }
};

// WARNING: These are downscaled and not centered
const std::vector<Vertex2D_UV> triangleVerticesUV =
{
  // XY          // UV
  { 0.00f, 0.0f, 0.0f, 0.0f },
  { 0.05f, 0.1f, 0.5f, 1.0f },
  { 0.10f, 0.0f, 1.0f, 0.0f }
};

// WARNING: These are downscaled and not centered
const std::vector<Vertex2D_RGB> triangleVerticesRGB =
{
  // XY          // RGB
  { 0.00f, 0.0f, 1.0f, 0.0f, 0.0f, },
  { 0.05f, 0.1f, 0.0f, 1.0f, 0.0f, },
  { 0.10f, 0.0f, 0.0f, 0.0f, 1.0f, }
};

const std::vector<unsigned int> triangleIndices =
{
  0, 1, 2
};

const std::vector<Vertex2D_Flat> quadVerticesFlat =
{ 
  // XY
  { -0.5f, -0.5f },
  {  0.5f, -0.5f },
  { -0.5f,  0.5f },
  {  0.5f,  0.5f }
};

const std::vector<unsigned int> quadIndices =
{ 
  0, 1, 2, // Bottom left
  1, 2, 3  // Top right
};

// WARNING: These are downscaled and not centered
const std::vector<Vertex2D_UV> fQuadVerticesUV =
{
  // XY           // UV
  { 0.00f, 0.00f,	0.0f, 0.0f },
  { 0.10f, 0.00f,	1.0f, 0.0f },
  { 0.05f, 0.05f, 0.5f, 0.5f },
  { 0.00f, 0.10f,	0.0f, 1.0f },
  { 0.10f, 0.10f,	1.0f, 1.0f }
};

// WARNING: These are downscaled and not centered
const std::vector<Vertex2D_RGB> fQuadVerticesRGB =
{
  // XY			      // RGB
  { 0.00f, 0.00f,	1.0f, 0.0f, 0.0f, }, // Bottom left
  { 0.10f, 0.00f,	0.0f, 1.0f, 0.0f, }, // Bottom right
  { 0.05f, 0.05f, 0.0f, 0.0f, 1.0f, }, // Center
  { 0.00f, 0.10f,	1.0f, 1.0f, 0.0f, }, // Top left
  { 0.10f, 0.10f,	1.0f, 1.0f, 1.0f, }  // Top right
};

const std::vector<unsigned int> fQuadIndices =
{
  0, 1, 2, // Bottom triangle
  1, 4, 2, // Right triangle
  2, 4, 3, // Top triangle
  0, 2, 3  // Left triangle
};

const std::vector<Vertex2D_Flat> rectangleVerticesFlat =
{ 
  // XY
  { -0.3f, -1.0f },
  {  0.3f, -1.0f },
  { -0.3f,  1.0f },
  {  0.3f,  1.0f }
};

const std::vector<unsigned int> rectangleIndices =
{ 
  0, 1, 2, // Bottom left
  1, 2, 3  // Top right
};

// ====
// GLFW
// ====

bool startGL(int width, int height, const char* title) 
{
  { // GLFW
    if (!glfwInit())
    {
      fprintf(stderr, "Error: Could not start GLFW3\n");
      return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    windowTitle = title;

    if (!window)
    {
      fprintf(stderr, "Error: Could not open window with GLFW3\n" );
      glfwTerminate();
      return false;
    }

    glfwMakeContextCurrent(window);

#if DISABLE_FPS_CAP
    glfwSwapInterval(0);
#endif
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

// =======
// Shaders
// =======

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
    fprintf(stderr, "Error: Shader %u did not compile\n", shader);
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
    fprintf(stderr, "Error: Program %u did not link\n", program);
    return false;
  }
  return true;
}

// ========
// Geometry
// ========

GLuint createBackgroundQuadVAO()
{
  GLuint VAO = 0, VBO = 0;

  float vertices[] =
  { // XY         // UV
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
  };
  
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glEnableVertexAttribArray(0);

  GLintptr stride = 4 * sizeof(float);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
  glEnableVertexAttribArray(1);
  
  GLintptr offset = 2 * sizeof(float);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*) offset);

  return VAO;
}

// ========
// Matrices
// ========

void setPositionMatrix(Matrix* matrix, const float x, const float y)
{
  /*
  1 0 0 0
  0 1 0 0
  0 0 1 0
  x y 0 1
  */
  matrix->a0 = 1;
  matrix->a1 = matrix->a2 = matrix->a3 = 0;

  matrix->b1 = 1;
  matrix->b0 = matrix->b2 = matrix->b3 = 0;

  matrix->c2 = 1;
  matrix->c0 = matrix->c1 = matrix->c3 = 0;

  matrix->d0 = x;
  matrix->d1 = y;
  matrix->d2 = 0;
  matrix->d3 = 1;
}

// =========
// Profiling
// =========

namespace
{
  int totalIterations = 0;
  // Times
  double totalElapsedTime = 0.0;
  double lastTime = 0.0;
  double dt = 0.0;
  // FPS
  double firstPreviousFPS = 0.0;
  double secondPreviousFPS = 0.0;
  double thirdPreviousFPS = 0.0;
  double fourthPreviousFPS = 0.0;
  double meanFPS = 0.0;
  // Title
  double lastTitleChangeTime = 0.0;
  double titleChangeDelta = 0.5;
  // Data export
  std::string dataFolderName;
  bool dataExported = false;
  int measureDataIterations = 100;
  int exportDataIterations = 1000;
  std::map<std::string,std::vector<std::pair<double, double>>> store;
}

void startProfiler(const std::string& name)
{
  dataFolderName = name;
}

void updateProfiler()
{
  totalIterations += 1;

  double currentTime = glfwGetTime();
  dt = currentTime - lastTime;
  lastTime = currentTime;

  if (dt == 0.0) return;
  
  totalElapsedTime += dt;

  fourthPreviousFPS = thirdPreviousFPS;
  thirdPreviousFPS = secondPreviousFPS;
  secondPreviousFPS = firstPreviousFPS;
  firstPreviousFPS = 1 / dt;

  meanFPS = (firstPreviousFPS + secondPreviousFPS + thirdPreviousFPS + fourthPreviousFPS) / 4.0;

  if (!dataExported && totalIterations > measureDataIterations) {
    store["FPS"].push_back(std::make_pair(meanFPS, totalIterations - measureDataIterations));
    store["Time"].push_back(std::make_pair(totalElapsedTime, totalIterations - measureDataIterations));
  }

  if (lastTime - lastTitleChangeTime >= titleChangeDelta)
  {
    lastTitleChangeTime = lastTime;

    std::string fpsString = std::format("{:.0f}", meanFPS);
    std::string titleString = windowTitle + " (" + fpsString + " FPS)";
    glfwSetWindowTitle(window, titleString.c_str());
  }

  if (totalIterations >= exportDataIterations + measureDataIterations && !dataExported)
  {
    exportData(resultPath(dataFolderName));
    dataExported = true;
  } 
}

double getDeltaTime()
{
  return dt;
}

double getFPS()
{
  return meanFPS;
}

void exportData(const std::filesystem::path& folderPath)
{
  std::filesystem::create_directories(folderPath);

  // For each vector of the store
  for (auto it = store.begin(); it != store.end(); ++it)
  {
    std::string key = it->first;
    std::vector<std::pair<double, double>> values = it->second;

    // Write into a representative csv file all the pairs from the vector
    std::string filePath = folderPath.string() + "/" + key + ".csv";
    std::ofstream file(filePath);
    for (auto vIt = values.begin(); vIt != values.end(); ++vIt)
    {
      double p1 = vIt->first;
      double p2 = vIt->second;
      if (file.is_open()) {
        file << p1 << "," << p2 << std::endl;
      } else {
        std::cout << "Failed to open the file: " << filePath << std::endl;
      }
    }

    file.close();
  }

  std::cout << "Data exported" << std::endl;
}