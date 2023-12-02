#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "gl_utils.h"
#include "path_manager.h"

// Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

const unsigned int INSTANCE_COUNT = 200000;
const float PARTICLE_SIZE = 0.01f;
const float PARTICLE_LIFE_TIME = 1.0f;
const float PARTICLE_HORIZONTAL_SPREAD = 2.0f;
const float PARTICLE_VERTICAL_IMPULSE = 4.0f;

namespace
{ // Unnamed namespace
  GLuint VAO(0);
  GLuint billboardQuadVBO(0);
  GLuint billboardQuadEBO(0);
  GLuint renderProgram(0);

  Particle particlesContainer[INSTANCE_COUNT];
  float particlesLifeTime;
} // Unnamed namespace

int main()
{
  startGL(WIDTH, HEIGHT, "Particles with DrawElements");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
  renderProgram = createRenderProgram(shaderPath("particle.vert"), shaderPath("particle.frag"));
  glUseProgram(renderProgram);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

	glGenBuffers(1, &billboardQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, billboardQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * billboardQuadVertices.size(), billboardQuadVertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2,	GL_FLOAT,	GL_FALSE,	2 * sizeof(float), (void*) 0);
  glVertexAttribDivisor(0, 0);

  glGenBuffers(1, &billboardQuadEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billboardQuadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * billboardQuadIndices.size(), billboardQuadIndices.data(), GL_STATIC_DRAW);

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

      for (int i = 0; i < INSTANCE_COUNT; i++)
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

      for (int i = 0; i < INSTANCE_COUNT; i++)
      {
        Particle& p = particlesContainer[i];

        // Simulate simple physics : gravity only, no collisions
        p.vel += glm::vec2(0.0f, -9.81f) * (float) delta;
        p.pos += p.vel * (float) delta;
      }
    }

    // Draw
    for (int i = 0; i < INSTANCE_COUNT; i++)
    {
      Particle& p = particlesContainer[i];

      // Setup per instance particle position
      glUniform2f(glGetUniformLocation(renderProgram, "particlePosition"), p.pos.x, p.pos.y);

      glDrawElements(
          GL_TRIANGLES, // Primitive type
          billboardQuadIndices.size(), // Amount of indices to use for the instance
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
  glDeleteBuffers(1, &billboardQuadVBO);
  glDeleteBuffers(1, &billboardQuadEBO);
  return 0;
}