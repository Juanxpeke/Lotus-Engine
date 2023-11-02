#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool startGL(int width, int height, char* title);
void stopGL();
std::string readShaderFile(const std::filesystem::path& shaderPath);
unsigned int compileShader(const std::string& shaderCode, unsigned int type);
bool checkShaderErrors(GLuint shader);
bool checkProgramErrors(GLuint program);
GLuint createQuadVAO();
GLuint createQuadProgram();

extern GLFWwindow* window;