// Based on: https://github.com/opengl-tutorials/ogl/blob/master/tutorial18_billboards_and_particles/tutorial18_particles.cpp
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
  GLuint particlesPositionsBuffer(0);
  GLuint renderProgram(0);

  Particle particlesContainer[INSTANCE_COUNT];
  float particlesLifeTime;
  float particlesPositionData[INSTANCE_COUNT * 2];
} // Unnamed namespace

int main()
{
  startGL(WIDTH, HEIGHT, "Particles with DrawElementsInstanced");

  // Set clear color
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Create and bind the shader program
  renderProgram = createRenderProgram(shaderPath("particle_ins_div.vert"), shaderPath("particle.frag"));
  glUseProgram(renderProgram);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

	glGenBuffers(1, &billboardQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, billboardQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * billboardQuadVertices.size(), billboardQuadVertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2,	GL_FLOAT,	GL_FALSE,	0, (void*) 0);
  glVertexAttribDivisor(0, 0);

  glGenBuffers(1, &billboardQuadEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billboardQuadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * billboardQuadIndices.size(), billboardQuadIndices.data(), GL_STATIC_DRAW);

	// The VBO containing the positions of the center of the particles
	glGenBuffers(1, &particlesPositionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particlesPositionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * 2 * sizeof(float), NULL, GL_STREAM_DRAW);

  glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
  glVertexAttribDivisor(1, 1);

  // Set particle size uniform
  glUniform1f(glGetUniformLocation(renderProgram, "particleSize"), PARTICLE_SIZE);

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

        // Fill the GPU buffer
        particlesPositionData[2 * i + 0] = p.pos.x;
        particlesPositionData[2 * i + 1] = p.pos.y;
      }
    }
      
    glBindBuffer(GL_ARRAY_BUFFER, particlesPositionsBuffer);
		glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * 2 * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming performance
		glBufferSubData(GL_ARRAY_BUFFER, 0, INSTANCE_COUNT * 2 * sizeof(float), particlesPositionData);

    // Draw
    glDrawElementsInstanced(
        GL_TRIANGLES, // Primitive type
        billboardQuadIndices.size(), // Amount of indices to use for each instance
        GL_UNSIGNED_INT, // Type of the indices
        (void*) (0 * sizeof(unsigned int)), // Offset into the index buffer object to begin reading data
        INSTANCE_COUNT); // Draw INSTANCE_COUNT objects

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