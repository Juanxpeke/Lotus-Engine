#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader_program.h"
#include "vertex_buffer.h"
#include "index_buffer.h"



int width = 720;
int height = 720;
char title[256];

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat vertices[] = {
    -1.0f,  1.0f, 0.0f, // Upper left corner
		-1.0f, -1.0f, 0.0f, // Lower left corner
		 1.0f, -1.0f, 0.0f, // Lower right corner
     1.0f,  1.0f, 0.0f, // Upper right corner
		-1.0f,  1.0f, 0.0f, // Upper left corner
     1.0f, -1.0f, 0.0f  // Lower right corner
	};

  sprintf(title, "OpenGL Render Engine");
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);


	ShaderProgram mvpShaderProgram("shaders/basic.vert", "shaders/basic.frag");

	// Create reference containers for the Vertex Array Object and the Vertex Buffer Object
	GLuint VAO;

	// Generate the VAO and VBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(glGetAttribLocation(mvpShaderProgram.getProgramID(), "position"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(glGetAttribLocation(mvpShaderProgram.getProgramID(), "position"));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glClearColor(0.27f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

  // Bind the VAO so OpenGL knows to use it
	glBindVertexArray(VAO);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		// Tell OpenGL which Shader Program we want to use
		mvpShaderProgram.bind();

		// Draw the triangle using the GL_TRIANGLES primitive
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
    
    glfwSwapBuffers(window);
	}

	// Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);

	glfwDestroyWindow(window);
	glfwTerminate();

	// TODO: See bug called when GL entities deleted after glfwTerminate (beacuse end of scope is after it)

  return 0;
}