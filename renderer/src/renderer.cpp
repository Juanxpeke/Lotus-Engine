#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"
#include "camera.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "path_manager.h"

int width = 720;
int height = 720;
char title[256];

GLfloat vertices[] =
{
	-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lower left corner
	0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lower right corner
	0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f // Upper corner
};

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

	camera = new Camera(window, width, height);

  // Set GLFW callbacks
  glfwSetCursorPosCallback(window, handleMouseMove);
  glfwSetMouseButtonCallback(window, handleMouseClick);

	ShaderProgram mvpShaderProgram(shaderPath("mvp.vert"), shaderPath("mvp2.frag"));
	mvpShaderProgram.bind();

	// Create reference containers for the Vertex Array Object and the Vertex Buffer Object
	GLuint VAO;
	GLuint VBO;
	// GLuint EBO;

	// Generate the VAO and VBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	
	
	// glVertexAttribPointer(glGetAttribLocation(mvpShaderProgram.getProgramID(), "position"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	// glEnableVertexAttribArray(glGetAttribLocation(mvpShaderProgram.getProgramID(), "position"));
	// // glVertexAttribPointer(glGetAttribLocation(mvpShaderProgram.getProgramID(), "normal"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
	// // glEnableVertexAttribArray(glGetAttribLocation(mvpShaderProgram.getProgramID(), "normal"));

	// // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindVertexArray(0);

	// glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// glClear(GL_COLOR_BUFFER_BIT);
	// glfwSwapBuffers(window);

  // // Bind the VAO so OpenGL knows to use it
	// glBindVertexArray(VAO);


	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


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

		glClear(GL_COLOR_BUFFER_BIT);

		// Tell OpenGL which Shader Program we want to use
		mvpShaderProgram.bind();

	  glUniformMatrix4fv(glGetUniformLocation(mvpShaderProgram.getProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(camera->view));
    glUniformMatrix4fv(glGetUniformLocation(mvpShaderProgram.getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(camera->projection));

		// Draw the triangle using the GL_TRIANGLES primitive
		//glDrawElements(GL_TRIANGLES, 4, GL_UNSIGNED_INT, nullptr);

		//glUseProgram(shaderProgram);

		glBindVertexArray(VAO);


		glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glfwSwapBuffers(window);
	}

	// Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);

	glfwDestroyWindow(window);
	glfwTerminate();

	// TODO: See bug called when GL classes auto deleted after glfwTerminate (beacuse end of scope is after it)

  return 0;
}