#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "gl_utils.h"
#include "path_manager.h"

#define SHADER_STORAGE_BUFFER true

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// When not using shader storage buffer, maximum is 4096 because of the UBO
const unsigned int DRAW_COUNT = 200000;
const float PARTICLE_SIZE = 0.01f;
const float PARTICLE_LIFE_TIME = 1.0f;
const float PARTICLE_HORIZONTAL_SPREAD = 2.0f;
const float PARTICLE_VERTICAL_IMPULSE = 4.0f;

namespace
{ // Unnamed namespace
  GLuint VAO(0);
  GLuint billboardQuadVBO(0);
  GLuint billboardQuadEBO(0);
  GLuint particlesPositionsBuffer(0);
  GLuint renderProgram(0);

  GLsizei indicesCounts[DRAW_COUNT];
  void* indicesOffsets[DRAW_COUNT];

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

int main()
{
  startGL(WIDTH, HEIGHT, "Particles with MultiDrawElements");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
#if SHADER_STORAGE_BUFFER
  renderProgram = createRenderProgram(shaderPath("particle_m_s.vert"), shaderPath("particle.frag"));
#else
  renderProgram = createRenderProgram(shaderPath("particle_m.vert"), shaderPath("particle.frag"));
#endif
  glUseProgram(renderProgram);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

	glGenBuffers(1, &billboardQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, billboardQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D_Flat) * quadVerticesFlat.size(), quadVerticesFlat.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2,	GL_FLOAT,	GL_FALSE,	2 * sizeof(float), (void*) 0);
  glVertexAttribDivisor(0, 0);

  glGenBuffers(1, &billboardQuadEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billboardQuadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quadIndices.size(), quadIndices.data(), GL_STATIC_DRAW);

#if SHADER_STORAGE_BUFFER
  // The SSBO containing the positions of the center of the particles
  glGenBuffers(1, &particlesPositionsBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesPositionsBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, DRAW_COUNT * sizeof(ParticlePositionData), NULL, GL_STREAM_DRAW);

  // Bind the shader storage buffer to index 0
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particlesPositionsBuffer);
#else
	// The UBO containing the positions of the center of the particles
	glGenBuffers(1, &particlesPositionsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, particlesPositionsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, DRAW_COUNT * sizeof(ParticlePositionData), NULL, GL_STREAM_DRAW);

  // Bind the uniform buffer to index 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, particlesPositionsBuffer);
#endif

  // Set particle size uniform
  glUniform1f(glGetUniformLocation(renderProgram, "particleSize"), PARTICLE_SIZE);

  // Set multiple data
  for (int i = 0; i < DRAW_COUNT; i++)
  {
    indicesCounts[i] = quadIndices.size();
    indicesOffsets[i] = (void*) (0 * sizeof(unsigned int));
  }

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

#if SHADER_STORAGE_BUFFER
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesPositionsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, DRAW_COUNT * sizeof(ParticlePositionData), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming performance
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, DRAW_COUNT * sizeof(ParticlePositionData), particlesPositionData);
#else
    glBindBuffer(GL_UNIFORM_BUFFER, particlesPositionsBuffer);
		glBufferData(GL_UNIFORM_BUFFER, DRAW_COUNT * sizeof(ParticlePositionData), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming performance
		glBufferSubData(GL_UNIFORM_BUFFER, 0, DRAW_COUNT * sizeof(ParticlePositionData), particlesPositionData);
#endif

    // Draw
    glMultiDrawElements(
        GL_TRIANGLES, // Primitive type
        indicesCounts, // Array of amount of indices to use for each drawing
        GL_UNSIGNED_INT, // Type of the indices
        indicesOffsets, // Array of offsets into the index buffer object to begin reading data
        DRAW_COUNT); // Draw DRAW_COUNT objects

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
  glDeleteBuffers(1, &billboardQuadVBO);
  glDeleteBuffers(1, &billboardQuadEBO);
  glDeleteBuffers(1, &particlesPositionsBuffer);
  return 0;
}