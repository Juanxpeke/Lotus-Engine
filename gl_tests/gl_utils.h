#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool startGL(int width, int height, char* title);
void stopGL();
std::string readShaderFile(const std::filesystem::path& shaderPath);
GLuint compileShader(const std::string& shaderCode, unsigned int type);
GLuint createRenderProgram(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);
GLuint createComputeProgram(const std::filesystem::path& computeShaderPath);
bool checkShaderErrors(GLuint shader);
bool checkProgramErrors(GLuint program);
GLuint createQuadVAO();

extern GLFWwindow* window;