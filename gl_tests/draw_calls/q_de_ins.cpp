#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_utils.h"
#include "path_manager.h"

#define BASE_INSTANCE false
#define VERTEX_ATTRIB_DIVISOR false

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// When not using vertex attrib divisor, maximum is 1024 because of the UBO
const unsigned int INSTANCE_COUNT = 100;

namespace 
{ // Unnamed namespace
  GLuint VAO(0);
  GLuint VBO(0);
  GLuint EBO(0);
  GLuint modelsBuffer(0);
  GLuint renderProgram(0);
} // Unnamed namespace

void generateGeometry()
{
  Matrix models[INSTANCE_COUNT];
  unsigned modelIndex(0);

  // Clipspace, lower left corner = (-1, -1)
  float xOffset(-0.95f);
  float yOffset(-0.95f);

  unsigned int instanceCountSqrt = sqrt(INSTANCE_COUNT);
  if (instanceCountSqrt * instanceCountSqrt != INSTANCE_COUNT)
  {
    std::cout << "Error: INSTANCE_COUNT has to be a perfect square" << std::endl;
    exit(1);
  }

  // Populate position matrices
  for (unsigned int i(0); i != instanceCountSqrt; ++i)
  {
    for (unsigned int j(0); j != instanceCountSqrt; ++j)
    {
      // Set position in model matrix
      setPositionMatrix(&models[modelIndex++], xOffset, yOffset);
      xOffset += 0.15f;
    }
    yOffset += 0.15f;
    xOffset = -0.95f;
  }

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D_RGB) * quadVerticesRGB.size(), quadVerticesRGB.data(), GL_STATIC_DRAW);

  // Specify vertex attributes for the shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D_RGB), (void*) (offsetof(Vertex2D_RGB, x)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D_RGB), (void*) (offsetof(Vertex2D_RGB, r)));

  // Create an element buffer and populate it
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quadIndices.size(), quadIndices.data(), GL_STATIC_DRAW);

  // Setup per instance matrices
#if VERTEX_ATTRIB_DIVISOR
  // Method 1. Use Vertex attributes and the vertex attrib divisor
  glGenBuffers(1, &modelsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, modelsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(models), &models, GL_STATIC_DRAW);

  // A matrix is 4 vec4s
  glEnableVertexAttribArray(3 + 0);
  glEnableVertexAttribArray(3 + 1);
  glEnableVertexAttribArray(3 + 2);
  glEnableVertexAttribArray(3 + 3);
  glVertexAttribPointer(3 + 0, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*) (offsetof(Matrix, a0)));
  glVertexAttribPointer(3 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*) (offsetof(Matrix, b0)));
  glVertexAttribPointer(3 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*) (offsetof(Matrix, c0)));
  glVertexAttribPointer(3 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*) (offsetof(Matrix, d0)));

  // Only apply one per instance
  glVertexAttribDivisor(3 + 0, 1);
  glVertexAttribDivisor(3 + 1, 1);
  glVertexAttribDivisor(3 + 2, 1);
  glVertexAttribDivisor(3 + 3, 1);
#else
  // Method 2. Use Uniform Buffers with gl_InstanceID. WARNING: We are not considering the std140 layout with
  // some fancy structure as the Matrix struct has 16 float values
	glGenBuffers(1, &modelsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, modelsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(models), &models, GL_STATIC_DRAW);

  // Bind the uniform buffer to index 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, modelsBuffer);
#endif
}

int main()
{
  startGL(WIDTH, HEIGHT, "Quads with DrawElementsInstanced");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
#if VERTEX_ATTRIB_DIVISOR
  renderProgram = createRenderProgram(shaderPath("flat_ins_div.vert"), shaderPath("flat.frag"));
#else
  renderProgram = createRenderProgram(shaderPath("flat_ins.vert"), shaderPath("flat.frag"));
#endif
  glUseProgram(renderProgram);

  generateGeometry();

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    updateProfiler();

    glClear(GL_COLOR_BUFFER_BIT);

    // Use program. Not needed in this example since we only have one that
    // we already use
    // glUseProgram(renderProgram);

    // Bind the vertex array we want to draw from. Not needed in this example
    // since we only have one that is already bounded
    // glBindVertexArray(VAO);

    // Draw
#if BASE_INSTANCE
    glDrawElementsInstancedBaseInstance(
        GL_TRIANGLES, // Primitive type
        quadIndices.size(), // Amount of indices to use for each instance
        GL_UNSIGNED_INT, // Type of the indices
        (void*) (0 * sizeof(unsigned int)), // Offset into the index buffer object to begin reading data
        INSTANCE_COUNT, // Draw INSTANCE_COUNT objects
        INSTANCE_COUNT / 2); // Base instance
#else
    glDrawElementsInstanced(
        GL_TRIANGLES, // Primitive type
        quadIndices.size(), // Amount of indices to use for each instance
        GL_UNSIGNED_INT, // Type of the indices
        (void*) (0 * sizeof(unsigned int)), // Offset into the index buffer object to begin reading data
        INSTANCE_COUNT); // Draw INSTANCE_COUNT objects
#endif

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(window, 1);
    }

    // GLFW: Swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // GLFW: Terminate, clearing all previously allocated GLFW resources.
  glfwTerminate();

  // Clean-up
  glDeleteProgram(renderProgram);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &modelsBuffer);
  return 0;
}