#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_utils.h"
#include "path_manager.h"

// Window stuff
char title[256];

// Dimensions of the image
int textureWidth = 512, textureHeight = 512;

// Work group stuff
int globalSize[3] = { textureWidth, textureHeight, 1 };
int localSize[3] = { 1, 1, 1 };

// Sets up the texture image
void setUpTextureImage(GLuint* textureID)
{
  glGenTextures(1, textureID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  // In order to write to a texture we use image storing functions in the shader. OpenGL treats "image units" slightly
  // differently to textures, so we call a glBindImageTexture() function to make this link. Note that we can set this
  // to "write only"
  glBindImageTexture(0, *textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

// Gets and prints the OpenGl work group data
void getWorkGroupData(int* workGrpCnt, int* workGrpSize, int& workGrpInv)
{
  // Check what the maximum size of the total work group that we give to glDispatchCompute() is
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGrpCnt[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGrpCnt[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGrpCnt[2]);
  printf("Maximum global (total) work group counts x:%i y:%i z:%i\n", workGrpCnt[0], workGrpCnt[1], workGrpCnt[2]);

  // We can also check the maximum size of a local work group (sub-division of the total number of jobs). This is 
  // defined in the compute shader itself, with the layout qualifier
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGrpSize[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGrpSize[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGrpSize[2]);
  printf("Maximum local (in one shader) work group sizes x:%i y:%i z:%i\n", workGrpSize[0], workGrpSize[1], workGrpSize[2]);

  // We can also determine the maximum number of work group units that a local work group in the compute shader is
  // allowed. This means that if we process a 32x32 tile of jobs in one local work group, then the product (1024) must
  // also not exceed this value
  glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGrpInv);
  printf("Maximum local work group invocations %i\n", workGrpInv);
}


int main()
{
  startGL(textureWidth, textureHeight, title);
  
  // Set up shaders and geometry
  GLuint renderProgram = createRenderProgram(shaderPath("background_quad.vert"), shaderPath("background_quad.frag"));
  GLuint computeProgram = createComputeProgram(shaderPath("raycast.comp"));
  GLuint quadVAO = createBackgroundQuadVAO();

  // Set up texture image
  GLuint textureID;
  setUpTextureImage(&textureID);

  // Work group data
  int workGrpCnt[3];
  int workGrpSize[3];
  int workGrpInv;
  getWorkGroupData(workGrpCnt, workGrpSize, workGrpInv);

  // Drawing loop
  while (!glfwWindowShouldClose(window))
  {

    { // Launch compute shaders
      glUseProgram(computeProgram);
      glDispatchCompute((GLuint) textureWidth, (GLuint) textureHeight, 1);
    }
    
    // Make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    { // Normal drawing pass
      glClear(GL_COLOR_BUFFER_BIT);
      glUseProgram(renderProgram);
      glBindVertexArray(quadVAO);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textureID);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    glfwPollEvents();

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(window, 1);
    }

    glfwSwapBuffers(window);
  }

  stopGL();

  return 0;
}