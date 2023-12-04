#pragma once

#include <filesystem>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#define DISABLE_FPS_CAP true

extern GLFWwindow* window;

struct Vertex2D_Flat
{
  float x, y; // Position
};

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

struct Particle
{
	glm::vec2 pos, vel;
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
extern const std::vector<Vertex2D_Flat> triangleVerticesFlat;
extern const std::vector<Vertex2D_UV> triangleVerticesUV;
extern const std::vector<Vertex2D_RGB> triangleVerticesRGB;
extern const std::vector<unsigned int> triangleIndices;
// Two triangles quad
extern const std::vector<Vertex2D_Flat> quadVerticesFlat;
extern const std::vector<unsigned int> quadIndices;
// Four triangles quad
extern const std::vector<Vertex2D_UV> fQuadVerticesUV;
extern const std::vector<Vertex2D_RGB> fQuadVerticesRGB;
extern const std::vector<unsigned int> fQuadIndices;
// Vertical rectangle
extern const std::vector<Vertex2D_Flat> rectangleVerticesFlat;
extern const std::vector<unsigned int> rectangleIndices;


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
void startProfiler(const std::string& name);
void updateProfiler();
double getDeltaTime();
double getFPS();
void exportData(const std::filesystem::path& folderPath);