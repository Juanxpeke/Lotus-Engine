#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "./scene/camera.h"
#include "./path_manager.h"
#include "traditional/mesh_manager.h"
#include "traditional/shader_program.h"
#include "traditional/diffuse_flat_material.h"
#include "traditional/mesh_instance.h"
#include "traditional/renderer.h"

int width = 720;
int height = 720;
char title[256];

void updateFromInputs(GLFWwindow* window, float dt, Camera* cameraPtr)
{
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getFrontVector() * dt * 8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getRightVector() * dt * -8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getFrontVector() * dt * -8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getRightVector() * dt * 8.f);
	}
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cameraPtr->translate(glm::vec3(0.0f, 1.0f, 0.0f) * dt * 8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    cameraPtr->translate(glm::vec3(0.0f, 1.0f, 0.0f) * dt * -8.f);
  }
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void createVent(Renderer& renderer)
{
	auto& meshManager = MeshManager::getInstance();
	std::shared_ptr<Mesh> ventMesh = meshManager.loadMesh(assetPath("models/air_conditioner/AirConditioner.obj").string());

	std::shared_ptr<DiffuseFlatMaterial> ventMaterial = std::static_pointer_cast<DiffuseFlatMaterial>(renderer.createMaterial(MaterialType::DiffuseFlat));

	ventMaterial->setDiffuseColor(glm::vec3(1.0f));

	MeshInstance* ventInstance = renderer.createMeshInstance(ventMesh, ventMaterial);

	ventInstance->scale(0.04f);

	float side = 50.f;
	float x = side * ((float)(rand()) / (float)(RAND_MAX)) - side / 2.0f;
	float y = side * ((float)(rand()) / (float)(RAND_MAX)) - side / 2.0f;
	float z = side * ((float)(rand()) / (float)(RAND_MAX)) - side / 2.0f;
	
	ventInstance->setTranslation(glm::vec3(x, y, z));
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

  Camera camera;

  Renderer renderer;
	renderer.startUp();

	const int objectCount = 5000; 

	for (int i = 0; i < objectCount; i++)
	{
		createVent(renderer);
	}
	
	float lastTime = -1.0f;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		float currentTime = glfwGetTime();

		if (lastTime >= 0.0f) {
			float dt = currentTime - lastTime;
			std::cout << "FPS: " << 1.0f / dt << std::endl;
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