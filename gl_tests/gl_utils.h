#pragma once

#include <filesystem>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define DISABLE_FPS_CAP true

extern GLFWwindow* window;

struct Vertex2D_UV
{
  float x, y; // Position
  float u, v; // Texture UV
};

struct Vertex2D_RGB
{
  float x, y; // Position
  float r, g, b; // Color
};

struct Matrix
{
  float a0, a1, a2, a3;
  float b0, b1, b2, b3;
  float c0, c1, c2, c3;
  float d0, d1, d2, d3;
};

struct DrawElementsCommand
{
  GLuint vertexCount;
  GLuint instanceCount;
  GLuint firstIndex;
  GLuint baseVertex;
  GLuint baseInstance;
};

// Triangle
extern const std::vector<Vertex2D_UV> triangleVerticesUV;
extern const std::vector<Vertex2D_RGB> triangleVerticesRGB;
extern const std::vector<unsigned int> triangleIndices;
// Four triangles quad
extern const std::vector<Vertex2D_UV> quadVerticesUV;
extern const std::vector<Vertex2D_RGB> quadVerticesRGB;
extern const std::vector<unsigned int> quadIndices;

// GLFW
bool startGL(int width, int height, const char* title);
void stopGL();
// Shaders
std::string readShaderFile(const std::filesystem::path& shaderPath);
GLuint compileShader(const std::string& shaderCode, unsigned int type);
GLuint createRenderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);
GLuint createComputeProgram(const std::filesystem::path& computeShaderPath);
bool checkShaderErrors(GLuint shader);
bool checkProgramErrors(GLuint program);
// Geometry
GLuint createBackgroundQuadVAO();
// Matrices
void setPositionMatrix(Matrix* matrix, const float x, const float y);
// Profiling
void updateProfiler();
double getFPS();