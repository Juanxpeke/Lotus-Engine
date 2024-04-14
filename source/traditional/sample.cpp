#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../scene/camera.h"
#include "../path_manager.h"
#include "mesh_manager.h"
#include "texture_manager.h"
#include "shader_program.h"
#include "diffuse_flat_material.h"
#include "diffuse_textured_material.h"
#include "mesh_instance.h"
#include "renderer.h"

int width = 720;
int height = 720;
char title[256];

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

void updateFromInputs(GLFWwindow* window, float dt, Camera* nptr)
{
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    nptr->getTransform().translate(nptr->getTransform().getFrontVector() * dt * 8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    nptr->getTransform().translate(nptr->getTransform().getRightVector() * dt * -8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    nptr->getTransform().translate(nptr->getTransform().getFrontVector() * dt * -8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    nptr->getTransform().translate(nptr->getTransform().getRightVector() * dt * 8.f);
	}
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    nptr->getTransform().translate(glm::vec3(0.0f, 1.0f, 0.0f) * dt * 8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    nptr->getTransform().translate(glm::vec3(0.0f, 1.0f, 0.0f) * dt * -8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
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

  Renderer renderer;
	renderer.startUp();
	
	auto& meshManager = MeshManager::getInstance();
	std::shared_ptr<Mesh> ventMesh = meshManager.loadMesh(assetPath("models/air_conditioner/AirConditioner.obj").string());
	
	auto& textureManager = TextureManager::getInstance();
	std::shared_ptr<Texture> ventTexture =  textureManager.loadTexture(assetPath("models/air_conditioner/Albedo.png").string());

	std::shared_ptr<DiffuseTexturedMaterial> ventMaterial = std::static_pointer_cast<DiffuseTexturedMaterial>(renderer.createMaterial(MaterialType::DiffuseTextured));

	ventMaterial->setMaterialTint(glm::vec3(1.0f));
	ventMaterial->setDiffuseTexture(ventTexture);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glfwSwapBuffers(window);

	float lastTime = -1.0f;
  
  Camera camera;

	renderer.createMeshInstance(ventMesh, ventMaterial);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		float currentTime = glfwGetTime();

		if (lastTime >= 0.0f) {
			float dt = currentTime - lastTime;
      updateFromInputs(window, dt, &camera);
		}

		lastTime = currentTime;

		renderer.render(camera);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}