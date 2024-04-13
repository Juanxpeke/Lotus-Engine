#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"
#include "mesh_manager.h"
#include "diffuse_flat_material.h"
#include "diffuse_textured_material.h"
#include "../scene/camera.h"
#include "../path_manager.h"

int width = 720;
int height = 720;
char title[256];

unsigned int m_lightDataUBO;

Camera* camera;

struct DirectionalLight
{
	// Paddings (https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL):
	// 12
	// 16
	// 28
	// 32
	glm::vec3 color;
	float padding04;
	glm::vec3 direction;
	float padding08;
};

struct Lights
{ 
	DirectionalLight directionalLights[2]; 
	glm::vec3 ambientLight;
	// PADDING?
	int directionalLightsCount; 
};

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

void lightSetup()
{
	glGenBuffers(1, &m_lightDataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightDataUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_lightDataUBO);
}

void renderLights()
{
	//Comienza carga en CPU de la informaci�n lum�nica de la escena
	Lights lights;
	lights.ambientLight = { 0.4, 0.4, 0.4 };

	//Se pasa la informacion de a lo mas las primeras NUM_HALF_MAX_DIRECTIONAL_LIGHTS * 2 componentes de luz direccional
	//A una instancia de Lights (informacion de la escena en CPU)
	uint32_t directionalLightsCount = 2;
	lights.directionalLightsCount = static_cast<int>(directionalLightsCount);
	for (uint32_t i = 0; i < directionalLightsCount; i++) {
		lights.directionalLights[i].color = { 0.4, 0.0, 0.4 };
		lights.directionalLights[i].direction = { 0.0, -1.0, 1.0 };
	}

	//Pasamos la informacion lum�nica a GPU con un unico llamado a OpenGL fuera de los loops de las primitivas.
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightDataUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), &lights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "GPU-Driven Rendering Engine");
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

	ShaderProgram mvpShaderProgram(shaderPath("diffuse_flat.vert"), shaderPath("diffuse_flat.frag"));
	DiffuseFlatMaterial dfm(mvpShaderProgram);
	
	ShaderProgram mvpSPT(shaderPath("diffuse_textured.vert"), shaderPath("diffuse_textured.frag"));
	DiffuseTexturedMaterial dtm(mvpSPT);

	lightSetup();

	auto& meshManager = MeshManager::getInstance();
	std::shared_ptr<Mesh> mesh = meshManager.loadMesh(Mesh::PrimitiveType::Sphere);
	std::shared_ptr<Mesh> amesh= meshManager.loadMesh(assetPath("models/air_conditioner/AirConditioner.obj").string());
	
	
	Texture* texturePtr = new Texture(assetPath("models/air_conditioner/Albedo.png").string());
	std::shared_ptr<Texture> textureSharedPtr = std::shared_ptr<Texture>(texturePtr);
	
	dtm.setMaterialTint(glm::vec3(1.0f));
	dtm.setDiffuseTexture(textureSharedPtr);

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


		renderLights();

		// dfm.setUniforms(camera->projection, camera->view, glm::mat4(1.0), camera->position);
    dtm.setUniforms(camera->projection, camera->view, glm::mat4(1.0), camera->position);
		glBindVertexArray(amesh->getVertexArrayID());
		glDrawElements(GL_TRIANGLES, amesh->getIndexBufferCount(), GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	// TODO: See bug called when GL classes auto deleted after glfwTerminate (beacuse end of scope is after it)

  return 0;
}