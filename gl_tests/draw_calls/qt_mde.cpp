// Based on: Jakob Törmä Ruhl
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_utils.h"
#include "path_manager.h"

#define BASE_VERTEX true

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// Maximum is 256 because of the shader layout
const unsigned int INSTANCE_COUNT = 100;

namespace // Unnamed namespace
{
  GLuint VAO(0);
  GLuint VBO(0);
  GLuint EBO(0);
  GLuint modelsBuffer(0);
  GLuint renderProgram(0);

  GLsizei indicesCounts[INSTANCE_COUNT];
  void* indicesOffsets[INSTANCE_COUNT];
#if BASE_VERTEX
  GLint baseVertices[INSTANCE_COUNT];
#endif

  float mouseX(0);
  float mouseY(0);

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

  // Populate geometry data
  for (unsigned int i(0); i != INSTANCE_COUNT; i++)
  {
    // Quad
    if (i % 2 == 0)
    {
      indicesCounts[i] = quadIndices.size();
      indicesOffsets[i] = (void*) (0 * sizeof(unsigned int));
#if BASE_VERTEX
      baseVertices[i] = 0;
#endif
    }
    // Triangle
    else {
      indicesCounts[i] = triangleIndices.size();
      indicesOffsets[i] = (void*) (quadIndices.size() * sizeof(unsigned int));
#if BASE_VERTEX
      baseVertices[i] = quadVertices.size();
#endif
    }

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

  int quadVerticesBytes = sizeof(Vertex2D) * quadVertices.size();
  int triangleVerticesBytes = sizeof(Vertex2D) * triangleVertices.size();

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, quadVerticesBytes + triangleVerticesBytes, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, quadVerticesBytes, quadVertices.data());
  glBufferSubData(GL_ARRAY_BUFFER, quadVerticesBytes, triangleVerticesBytes, triangleVertices.data());

  // Specify vertex attributes for the shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*) (offsetof(Vertex2D, x)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*) (offsetof(Vertex2D, r)));

  // Create an element buffer and populate it
  int quadIndicesBytes = sizeof(unsigned int) * quadIndices.size();
  int triangleIndicesBytes = sizeof(unsigned int) * triangleIndices.size();

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBytes + triangleIndicesBytes, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quadIndicesBytes, quadIndices.data());
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBytes, triangleIndicesBytes, triangleIndices.data());

  // Setup per instance matrices
  // Method 1. Use Vertex attributes and the vertex attrib divisor (This doesn't work in this case as we
  // have no way of separating instances with MultiDrawElements, we only can separate draws)

  // Method 2. Use Uniform Buffers with gl_DrawID. WARNING: We are not considering the std140 layout with
  // some fancy structure as the Matrix struct has 16 float values
	glGenBuffers(1, &modelsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, modelsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(models), &models, GL_STATIC_DRAW);

  // Bind the uniform buffer to index 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, modelsBuffer);
}

int main()
{
  startGL(WIDTH, HEIGHT, "Quads and triangles with MultiDrawElements");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
  renderProgram = createRenderProgram(shaderPath("flat_m.vert"), shaderPath("flat.frag"));
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
#if BASE_VERTEX
    glMultiDrawElementsBaseVertex(
        GL_TRIANGLES, // Primitive type
        indicesCounts, // Array of amount of indices to use for each drawing
        GL_UNSIGNED_INT, // Type of the indices
        indicesOffsets, // Array of offsets into the index buffer object to begin reading data
        INSTANCE_COUNT, // Draw INSTANCE_COUNT objects
        baseVertices); // Array of values added to each index before pulling from the vertex data
#else
    // This is not useful as you would need to define custom indices for each element as the VBO is shared
    // Ex: If the first vertices in the VBO are of the quad, the indices for the triangle can't start at 0
    glMultiDrawElements(
        GL_TRIANGLES, // Primitive type
        indicesCounts, // Array of amount of indices to use for each drawing
        GL_UNSIGNED_INT, // Type of the indices
        indicesOffsets, // Array of offsets into the index buffer object to begin reading data
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