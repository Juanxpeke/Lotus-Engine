#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_utils.h"
#include "path_manager.h"

#define INSTANCING true

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// Maximum is 1024 because of the UBO
const unsigned int DRAW_COUNT = 100;

namespace
{ // Unnamed namespace
  GLuint VAO(0);
  GLuint VBO(0);
  GLuint EBO(0);
  GLuint indirectBuffer(0);
  GLuint modelsBuffer(0);
  GLuint renderProgram(0);
} // Unnamed namespace

void generateGeometry()
{
  Matrix models[DRAW_COUNT];
  unsigned modelIndex(0);

  // Clipspace, lower left corner = (-1, -1)
  float xOffset(-0.95f);
  float yOffset(-0.95f);

  unsigned int instanceCountSqrt = sqrt(DRAW_COUNT);
  if (instanceCountSqrt * instanceCountSqrt != DRAW_COUNT)
  {
    std::cout << "Error: DRAW_COUNT has to be a perfect square" << std::endl;
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

  int quadVerticesBytes = sizeof(Vertex2D_RGB) * quadVerticesRGB.size();
  int triangleVerticesBytes = sizeof(Vertex2D_RGB) * triangleVerticesRGB.size();

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, quadVerticesBytes + triangleVerticesBytes, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, quadVerticesBytes, quadVerticesRGB.data());
  glBufferSubData(GL_ARRAY_BUFFER, quadVerticesBytes, triangleVerticesBytes, triangleVerticesRGB.data());

  // Specify vertex attributes for the shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D_RGB), (void*) (offsetof(Vertex2D_RGB, x)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D_RGB), (void*) (offsetof(Vertex2D_RGB, r)));

  // Create an element buffer and populate it
  int quadIndicesBytes = sizeof(unsigned int) * quadIndices.size();
  int triangleIndicesBytes = sizeof(unsigned int) * triangleIndices.size();

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBytes + triangleIndicesBytes, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quadIndicesBytes, quadIndices.data());
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBytes, triangleIndicesBytes, triangleIndices.data());

  // Setup per instance matrices
#if INSTANCING
  // Use Uniform Buffers with gl_DrawID and gl_InstanceID. WARNING: We are not considering the std140 layout with
  // some fancy structure as the Matrix struct has 16 float values
#else
  // Use Uniform Buffers with gl_DrawID. WARNING: We are not considering the std140 layout with
  // some fancy structure as the Matrix struct has 16 float values
#endif
	glGenBuffers(1, &modelsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, modelsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(models), &models, GL_STATIC_DRAW);

  // Bind the uniform buffer to index 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, modelsBuffer);

}

void generateDrawCommands()
{
#if INSTANCING
  DrawElementsCommand drawCommands[2];

  drawCommands[0].vertexCount = quadIndices.size(); // Amount of indices to use for each instance
  drawCommands[0].instanceCount = DRAW_COUNT / 2; // Draw DRAW_COUNT / 2 instances
  drawCommands[0].firstIndex = 0; // Offset into the index buffer object to begin reading data
  drawCommands[0].baseVertex = 0; // Value added to each index before pulling from the vertex data
  drawCommands[0].baseInstance = 0; // Base instance

  drawCommands[1].vertexCount = triangleIndices.size(); // Amount of indices to use for each instance
  drawCommands[1].instanceCount = DRAW_COUNT / 2; // Draw DRAW_COUNT / 2 instances
  drawCommands[1].firstIndex = quadIndices.size(); // Offset into the index buffer object to begin reading data
  drawCommands[1].baseVertex = quadVerticesRGB.size(); // Value added to each index before pulling from the vertex data
  drawCommands[1].baseInstance = 0; // Base instance
#else
  // Generate draw commands
  DrawElementsCommand drawCommands[DRAW_COUNT];

  for (unsigned int i(0); i < DRAW_COUNT; ++i)
  {
    // Quad
    if (i % 2 == 0)
    {
      drawCommands[i].vertexCount = quadIndices.size(); // Amount of indices to use for each instance
      drawCommands[i].instanceCount = 1; // Draw 1 instance
      drawCommands[i].firstIndex = 0; // Offset into the index buffer object to begin reading data
      drawCommands[i].baseVertex = 0; // Value added to each index before pulling from the vertex data
      drawCommands[i].baseInstance = 0; // Base instance
    }
    // Triangle
    else
    {
      drawCommands[i].vertexCount = triangleIndices.size(); // Amount of indices to use for each instance
      drawCommands[i].instanceCount = 1; // Draw 1 instance
      drawCommands[i].firstIndex = quadIndices.size(); // Offset into the index buffer object to begin reading data
      drawCommands[i].baseVertex = quadVerticesRGB.size(); // Value added to each index before pulling from the vertex data
      drawCommands[i].baseInstance = 0; // Base instance
    }
  }
#endif
  // Feed the draw command data to the GPU
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommands), drawCommands, GL_DYNAMIC_DRAW);
}

int main()
{
  startGL(WIDTH, HEIGHT, "Quads and triangles with MultiDrawElementsIndirect");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
#if INSTANCING
  renderProgram = createRenderProgram(shaderPath("flat_m_ind.vert"), shaderPath("flat.frag"));
#else
  renderProgram = createRenderProgram(shaderPath("flat_m.vert"), shaderPath("flat.frag"));
#endif
  glUseProgram(renderProgram);

  generateGeometry();

  // Generate one indirect draw buffer
  glGenBuffers(1, &indirectBuffer);

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

    generateDrawCommands();

    // Draw
#if INSTANCING
    glMultiDrawElementsIndirect(
        GL_TRIANGLES, // Primitive type
        GL_UNSIGNED_INT, // Type of the indices
        (GLvoid*) 0, // Offset into the indirect buffer object to begin reading commands
        2, // Make 2 draws of DRAW_COUNT /  2 instances
        0); // No stride, the draw commands are tightly packed
#else
    glMultiDrawElementsIndirect(
        GL_TRIANGLES, // Primitive type
        GL_UNSIGNED_INT, // Type of the indices
        (GLvoid*) 0, // Offset into the indirect buffer object to begin reading commands
        DRAW_COUNT, // Make DRAW_COUNT draws of 1 instance
        0); // No stride, the draw commands are tightly packed
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
  glDeleteBuffers(1, &indirectBuffer);
  return 0;
}