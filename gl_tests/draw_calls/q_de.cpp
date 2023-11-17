// Based on: Jakob Törmä Ruhl
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_utils.h"
#include "path_manager.h"

#define BASE_INSTANCE false

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

const unsigned int INSTANCE_COUNT = 100;

namespace
{ // Unnamed namespace
  GLuint VAO(0);
  GLuint VBO(0);
  GLuint EBO(0);
  GLuint renderProgram(0);

  Matrix models[INSTANCE_COUNT];
} // Unnamed namespace

void generateGeometry()
{
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
}

int main()
{
  startGL(WIDTH, HEIGHT, "Quads with DrawElements");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
  renderProgram = createRenderProgram(shaderPath("flat.vert"), shaderPath("flat.frag"));
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
    for (int i = 0; i < INSTANCE_COUNT; i++)
    {
      // Setup per instance matrices
      glUniformMatrix4fv(glGetUniformLocation(renderProgram, "model"), 1, GL_FALSE, (GLfloat*) &models[i]);

      glDrawElements(
          GL_TRIANGLES, // Primitive type
          quadIndices.size(), // Amount of indices to use for the instance
          GL_UNSIGNED_INT, // Type of the indices
          (void*) (0 * sizeof(unsigned int))); // Offset into the index buffer object to begin reading data
    }

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
  return 0;
}