#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "gl_utils.h"
#include "path_manager.h"

#define INSTANCING true

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// When not using instancing, you can't draw thousands of particles because
// that would mean thousands of draw commands and lead to an stack overflow,
// besices, when not using shader storage buffer, maximum is 4096 because of
// the UBO
const unsigned int DRAW_COUNT = 7;
const float PARTICLE_SIZE = 0.1f;
const float PARTICLE_LIFE_TIME = 1.0f;
const float PARTICLE_HORIZONTAL_SPREAD = 2.0f;
const float PARTICLE_VERTICAL_IMPULSE = 4.0f;

namespace
{ // Unnamed namespace
  GLuint VAO(0);
  GLuint billboardsVBO(0);
  GLuint billboardsEBO(0);
  GLuint indirectBuffer(0);
  GLuint particlesPositionsBuffer(0);
  GLuint renderProgram(0);

  Particle particlesContainer[DRAW_COUNT];
  float particlesLifeTime;

  struct ParticlePositionData
  { 
    // The UBO will be represented as an array of vec2, so each element will
    // have the same size of a vec4 (16 bytes for GLSL) based on std140 layout
    float x;
    float y;
    float padding04;
    float padding08;
  };

  ParticlePositionData particlesPositionData[DRAW_COUNT];
} // Unnamed namespace

void generateDrawCommands()
{
  // Generate draw commands
#if INSTANCING
  DrawElementsCommand drawCommands[2];

  drawCommands[0].vertexCount = quadIndices.size(); // Amount of indices to use for each instance
  drawCommands[0].instanceCount = DRAW_COUNT / 2; // Draw DRAW_COUNT / 3 instances
  drawCommands[0].firstIndex = 0; // Offset into the index buffer object to begin reading data
  drawCommands[0].baseVertex = 0; // Value added to each index before pulling from the vertex data
  drawCommands[0].baseInstance = 0; // Base instance

  drawCommands[1].vertexCount = triangleIndices.size(); // Amount of indices to use for each instance
  drawCommands[1].instanceCount = DRAW_COUNT / 2; // Draw DRAW_COUNT / 3 instances
  drawCommands[1].firstIndex = quadIndices.size(); // Offset into the index buffer object to begin reading data
  drawCommands[1].baseVertex = quadVerticesFlat.size(); // Value added to each index before pulling from the vertex data
  drawCommands[1].baseInstance = 0; // Base instance

  //drawCommands[2].vertexCount = billboardRectangleIndices.size(); // Amount of indices to use for each instance
  //drawCommands[2].instanceCount = DRAW_COUNT / 3; // Draw DRAW_COUNT / 3 instances
  //drawCommands[2].firstIndex = quadIndices.size() + billboardTriangleIndices.size(); // Offset into the index buffer object to begin reading data
  //drawCommands[2].baseVertex = quadVerticesFlat.size() + triangleVerticesFlat.size(); // Value added to each index before pulling from the vertex data
  //drawCommands[2].baseInstance = 0; // Base instance
#else
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
      drawCommands[i].baseVertex = quadVerticesFlat.size(); // Value added to each index before pulling from the vertex data
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
  startGL(WIDTH, HEIGHT, "Different particles with MultiDrawElementsIndirect");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
#if INSTANCING
    renderProgram = createRenderProgram(shaderPath("particle_m_ind.vert"), shaderPath("particle.frag"));
#else
    renderProgram = createRenderProgram(shaderPath("particle_m_s.vert"), shaderPath("particle.frag"));
#endif
  glUseProgram(renderProgram);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  int quadVerticesBytes = sizeof(Vertex2D_Flat) * quadVerticesFlat.size();
  int triangleVerticesBytes = sizeof(Vertex2D_Flat) * triangleVerticesFlat.size();
  int rectangleVerticesBytes = sizeof(Vertex2D_Flat) * billboardRectangleVertices.size();

  glGenBuffers(1, &billboardsVBO);
  glBindBuffer(GL_ARRAY_BUFFER, billboardsVBO);
  glBufferData(GL_ARRAY_BUFFER, quadVerticesBytes + triangleVerticesBytes, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, quadVerticesBytes, quadVerticesFlat.data());
  glBufferSubData(GL_ARRAY_BUFFER, quadVerticesBytes, triangleVerticesBytes, triangleVerticesFlat.data());
  //glBufferSubData(GL_ARRAY_BUFFER, quadVerticesBytes + triangleVerticesBytes, rectangleVerticesBytes, billboardRectangleVertices.data());

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2,	GL_FLOAT,	GL_FALSE,	2 * sizeof(float), (void*) 0);
  glVertexAttribDivisor(0, 0);

  int quadIndicesBytes = sizeof(unsigned int) * quadIndices.size();
  int triangleIndicesBytes = sizeof(unsigned int) * triangleIndices.size();
  int rectangleIndicesBytes = sizeof(unsigned int) * billboardRectangleIndices.size();

  glGenBuffers(1, &billboardsEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billboardsEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBytes + triangleIndicesBytes, NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quadIndicesBytes, quadIndices.data());
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBytes, triangleIndicesBytes, triangleIndices.data());
  //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBytes + triangleIndicesBytes, rectangleIndicesBytes, billboardRectangleIndices.data());

  // The SSBO containing the positions of the center of the particles
  glGenBuffers(1, &particlesPositionsBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesPositionsBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, DRAW_COUNT * sizeof(ParticlePositionData), NULL, GL_STREAM_DRAW);

  // Bind the shader storage buffer to index 0
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particlesPositionsBuffer);

  // Generate one indirect draw buffer
  glGenBuffers(1, &indirectBuffer);

  // Set particle size uniform
  glUniform1f(glGetUniformLocation(renderProgram, "particleSize"), PARTICLE_SIZE);

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    updateProfiler();

    glClear(GL_COLOR_BUFFER_BIT);

    float delta = getDeltaTime();
		
    if (particlesLifeTime <= 0.0f)
    {
      particlesLifeTime = PARTICLE_LIFE_TIME;

      for (int i = 0; i < DRAW_COUNT; i++)
      {
        Particle& p = particlesContainer[i];

        p.pos = glm::vec2(0, 0);

        // Horizontal speed between -PARTICLE_HORIZONTAL_SPREAD and PARTICLE_HORIZONTAL_SPREAD
        float horizontalSpeed = (rand() % 2000 - 1000.0f) / 1000.0f * PARTICLE_HORIZONTAL_SPREAD;

        // Vertical speed between 0 and PARTICLE_VERTICAL_IMPULSE
        float verticalSpeed = (rand() % 1000) / 1000.0f * PARTICLE_VERTICAL_IMPULSE;
        
        p.vel = glm::vec2(horizontalSpeed, verticalSpeed);
      }
		}
    else
    {
      particlesLifeTime -= delta;

      for (int i = 0; i < DRAW_COUNT; i++)
      {
        Particle& p = particlesContainer[i];

        // Simulate simple physics : gravity only, no collisions
        p.vel += glm::vec2(0.0f, -9.81f) * (float) delta;
        p.pos += p.vel * (float) delta;

        // Fill the GPU buffer
        particlesPositionData[i].x = p.pos.x;
        particlesPositionData[i].y = p.pos.y;
      }
    }
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesPositionsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, DRAW_COUNT * sizeof(ParticlePositionData), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming performance
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, DRAW_COUNT * sizeof(ParticlePositionData), particlesPositionData);

    generateDrawCommands();

    // Draw
#if INSTANCING
    glMultiDrawElementsIndirect(
        GL_TRIANGLES, // Primitive type
        GL_UNSIGNED_INT, // Type of the indices
        (GLvoid*) 0, // Offset into the indirect buffer object to begin reading commands
        2, // Make 3 draws of DRAW_COUNT / 3 instances
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
  glDeleteBuffers(1, &billboardsVBO);
  glDeleteBuffers(1, &billboardsEBO);
  glDeleteBuffers(1, &indirectBuffer);
  glDeleteBuffers(1, &particlesPositionsBuffer);
  return 0;
}