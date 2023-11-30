#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_utils.h"
#include "path_manager.h"

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

namespace
{ // Unnamed namespace
  GLuint gVAO(0);
  GLuint gArrayTexture(0);
  GLuint VBO(0);
  GLuint EBO(0);
  GLuint gIndirectBuffer(0);
  GLuint matrixBuffer(0);
  GLuint renderProgram(0);
} // Unnamed namespace

void generateGeometry()
{
  // Generate 50 quads, 50 triangles
  const unsigned numVertices = quadVerticesUV.size() * 50 + triangleVerticesUV.size() * 50;
  std::vector<Vertex2D_UV> vVertex(numVertices);

  Matrix vMatrix[100];

  unsigned vertexIndex(0);
  unsigned matrixIndex(0);

  // Clipspace, lower left corner = (-1, -1)
  float xOffset(-0.95f);
  float yOffset(-0.95f);

  // Populate geometry
  for (unsigned int i(0); i != 10; ++i)
  {
    for (unsigned int j(0); j != 10; ++j)
    {
      // Quad
      if (j % 2 == 0)
      {
        for (unsigned int k(0); k != quadVerticesUV.size(); ++k)
        {
          vVertex[vertexIndex++] = quadVerticesUV[k];
        }
      }
      // Triangle
      else
      {
        for (unsigned int k(0); k != triangleVerticesUV.size(); ++k)
        {
          vVertex[vertexIndex++] = triangleVerticesUV[k];
        }
      }

      // Set position in model matrix
      setPositionMatrix(&vMatrix[matrixIndex++], xOffset, yOffset);
      xOffset += 0.2f;
    }
    yOffset += 0.2f;
    xOffset = -0.95f;
  }

  glGenVertexArrays(1, &gVAO);
  glBindVertexArray(gVAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D_UV) * vVertex.size(), vVertex.data(), GL_STATIC_DRAW);

  // Specify vertex attributes for the shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D_UV), (GLvoid*) (offsetof(Vertex2D_UV, x)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D_UV), (GLvoid*) (offsetof(Vertex2D_UV, u)));

  // Create an element buffer and populate it
  int triangleBytes = sizeof(unsigned int) * triangleIndices.size();
  int quadBytes = sizeof(unsigned int) * quadIndices.size();

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleBytes + quadBytes, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quadBytes, quadIndices.data());
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, quadBytes, triangleBytes, triangleIndices.data());

  // Setup per instance matrices
  // Method 1. Use Vertex attributes and the vertex attrib divisor
  glGenBuffers(1, &matrixBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vMatrix), vMatrix, GL_STATIC_DRAW);
  // A matrix is 4 vec4s
  glEnableVertexAttribArray(3 + 0);
  glEnableVertexAttribArray(3 + 1);
  glEnableVertexAttribArray(3 + 2);
  glEnableVertexAttribArray(3 + 3);

  glVertexAttribPointer(3 + 0, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*) (offsetof(Matrix, a0)));
  glVertexAttribPointer(3 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*) (offsetof(Matrix, b0)));
  glVertexAttribPointer(3 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*) (offsetof(Matrix, c0)));
  glVertexAttribPointer(3 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*) (offsetof(Matrix, d0)));
  // Only apply one per instance
  glVertexAttribDivisor(3 + 0, 1);
  glVertexAttribDivisor(3 + 1, 1);
  glVertexAttribDivisor(3 + 2, 1);
  glVertexAttribDivisor(3 + 3, 1);

  // Method 2. Use Uniform Buffers. Not shown here
}

void generateArrayTexture()
{
  // Generate an array texture
  glGenTextures(1, &gArrayTexture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, gArrayTexture);

  // Create storage for the texture. (100 layers of 1x1 texels)
  glTexStorage3D(
      GL_TEXTURE_2D_ARRAY,
      1, // No mipmaps as textures are 1x1
      GL_RGB8, // Internal format
      1, 1, // Width, height
      100); //Number of layers

  for (unsigned int i(0); i != 100; ++i)
  {
    // Choose a random color for the i-essim image
    GLubyte color[3] = { GLubyte(rand() % 255),GLubyte(rand() % 255),GLubyte(rand() % 255) };

    // Specify i-essim image
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY,
        0, // Mipmap number
        0, 0, i, // xoffset, yoffset, zoffset
        1, 1, 1, // Width, height, depth
        GL_RGB, // Format
        GL_UNSIGNED_BYTE, // Type
        color); // Pointer to data
  }

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void generateDrawCommands()
{
  // Generate draw commands
  DrawElementsCommand vDrawCommand[100];
  GLuint baseVert = 0;
  for (unsigned int i(0); i < 100; ++i)
  {
    // Quad
    if (i % 2 == 0)
    {
      vDrawCommand[i].vertexCount = quadIndices.size(); // 4 triangles = 12 vertices
      vDrawCommand[i].instanceCount = 1; // Draw 1 instance
      vDrawCommand[i].firstIndex = 0; // Draw from index 0 for this instance
      vDrawCommand[i].baseVertex = baseVert; // Starting from baseVert
      vDrawCommand[i].baseInstance = i; // gl_InstanceID
      baseVert += quadVerticesUV.size();
    }
    // Triangle
    else
    {
      vDrawCommand[i].vertexCount = triangleIndices.size(); // 1 triangle = 3 vertices
      vDrawCommand[i].instanceCount = 1; // Draw 1 instance
      vDrawCommand[i].firstIndex = 0; // Draw from index 0 for this instance
      vDrawCommand[i].baseVertex = baseVert; // Starting from baseVert
      vDrawCommand[i].baseInstance = i; // gl_InstanceID
      baseVert += triangleVerticesUV.size();
    }
  }

  // Feed the draw command data to the GPU
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gIndirectBuffer);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(vDrawCommand), vDrawCommand, GL_DYNAMIC_DRAW);

  // Feed the instance id to the shader. TODO: Review this and the entire program
  glBindBuffer(GL_ARRAY_BUFFER, gIndirectBuffer);
  glEnableVertexAttribArray(2);
  glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(DrawElementsCommand), (void*) (offsetof(DrawElementsCommand, baseInstance)));
  glVertexAttribDivisor(2, 1); // Only once per instance
}

int main()
{
  startGL(WIDTH, HEIGHT, "Quads and triangles with MultiDrawElementsIndirect");

  // Set clear color
  glClearColor(1.0, 1.0, 1.0, 0.0);

  // Create and bind the shader program
  renderProgram = createRenderProgram(shaderPath("rect.vert"), shaderPath("rect.frag"));
  glUseProgram(renderProgram);

  generateGeometry();
  generateArrayTexture();

  // Set the sampler for the texture.
  // Hacky but we know that the arraysampler is at bindingpoint 0.
  glUniform1i(0, 0);

  // Generate one indirect draw buffer
  glGenBuffers(1, &gIndirectBuffer);

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);

    // Use program. Not needed in this example since we only have one that
    // we already use
    // glUseProgram(renderProgram);

    // Bind the vertex array we want to draw from. Not needed in this example
    // since we only have one that is already bounded
    // glBindVertexArray(gVAO);

    generateDrawCommands();

    // Draw
    glMultiDrawElementsIndirect(
        GL_TRIANGLES, // Type
        GL_UNSIGNED_INT, // Indices represented as unsigned ints
        (GLvoid*) 0, // Start with the first draw command
        100, // Draw 100 objects
        0); // No stride, the draw commands are tightly packed


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
  glDeleteVertexArrays(1, &gVAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &matrixBuffer);
  glDeleteBuffers(1, &gIndirectBuffer);
  glDeleteTextures(1, &gArrayTexture);
  return 0;
}