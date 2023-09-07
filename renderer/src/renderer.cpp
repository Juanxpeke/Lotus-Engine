#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"
#include "mesh.h"
#include "camera.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "path_manager.h"

int width = 720;
int height = 720;
char title[256];

Camera* camera;

// Function to handle mouse movement
void handleMouseMove(GLFWwindow* window, double xPos, double yPos)
{
  camera->handleMouseMove(xPos, yPos);
}

// Function to handle mouse button events
void handleMouseClick(GLFWwindow* window, int button, int action, int mods)
{
  camera->handleMouseClick(button, action, mods);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "OpenGL Rendering Engine");
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

	camera = new Camera(window, width, height);

  // Set GLFW callbacks
  glfwSetCursorPosCallback(window, handleMouseMove);
  glfwSetMouseButtonCallback(window, handleMouseClick);

	ShaderProgram mvpShaderProgram(shaderPath("mvp.vert"), shaderPath("mvp2.frag"));
	mvpShaderProgram.bind();

	Mesh mesh(Mesh::PrimitiveType::Sphere);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glfwSwapBuffers(window);

	float lastTime = -1.0f;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		float currentTime = glfwGetTime();

		if (lastTime >= 0.0f) {
			float dt = currentTime - lastTime;
			camera->update(dt);
		}

		lastTime = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mvpShaderProgram.bind();

	  glUniformMatrix4fv(glGetUniformLocation(mvpShaderProgram.getProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(camera->view));
    glUniformMatrix4fv(glGetUniformLocation(mvpShaderProgram.getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(camera->projection));

    glBindVertexArray(mesh.getVertexArrayID());
		glDrawElements(GL_TRIANGLES, mesh.getIndexBufferCount(), GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	// TODO: See bug called when GL classes auto deleted after glfwTerminate (beacuse end of scope is after it)

  return 0;
}