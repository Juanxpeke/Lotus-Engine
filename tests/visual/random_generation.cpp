#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "math/types.h"
#include "util/path_manager.h"
#include "scene/camera.h"
#include "render/indirect/renderer.h"
#include "render/mesh_manager.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 14.4f;
const float cameraAngularSpeed = 2.0f;

const int objectsCount = 32000;
const float objectsAreaSide = 160.f;

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
std::shared_ptr<Lotus::Mesh> ventMesh;

void updateFromInputs(GLFWwindow* window, float dt, Lotus::Camera* cameraPtr)
{
  // Translation
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getFrontVector() * dt * cameraSpeed);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getRightVector() * dt * -cameraSpeed);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getFrontVector() * dt * -cameraSpeed);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cameraPtr->translate(cameraPtr->getRightVector() * dt * cameraSpeed);
	}
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cameraPtr->translate(glm::vec3(0.0f, 1.0f, 0.0f) * dt * cameraSpeed);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    cameraPtr->translate(glm::vec3(0.0f, 1.0f, 0.0f) * dt * -cameraSpeed);
  }
  // Rotation
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    cameraPtr->rotate(cameraPtr->getRightVector(), dt * cameraAngularSpeed);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    cameraPtr->rotate(glm::vec3(0.0f, 1.0f, 0.0f), dt * cameraAngularSpeed);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    cameraPtr->rotate(cameraPtr->getRightVector(), dt * -cameraAngularSpeed);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    cameraPtr->rotate(glm::vec3(0.0f, 1.0f, 0.0f), dt * -cameraAngularSpeed);
  }
  // Misc
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void createDirectionalLight(Lotus::Renderer& renderer)
{
  std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderer.createDirectionalLight();

  directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::pi<float>() * 0.5f);
  directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::pi<float>() * 0.25f);
  directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
}

void createVent(Lotus::Renderer& renderer)
{
	std::shared_ptr<Lotus::DiffuseFlatMaterial> ventMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderer.createMaterial(Lotus::MaterialType::DiffuseFlat));

	float r = ((float)(std::rand()) / (float)(RAND_MAX));
	float g = ((float)(std::rand()) / (float)(RAND_MAX));
	float b = ((float)(std::rand()) / (float)(RAND_MAX));

	ventMaterial->setDiffuseColor(glm::vec3(r, g, b));

	std::shared_ptr<Lotus::MeshInstance> ventInstance = renderer.createMeshInstance(ventMesh, ventMaterial);

	float x = objectsAreaSide * ((float)(std::rand()) / (float)(RAND_MAX)) - objectsAreaSide / 2.0f;
	float y = objectsAreaSide * ((float)(std::rand()) / (float)(RAND_MAX)) - objectsAreaSide / 2.0f;
	float z = objectsAreaSide * ((float)(std::rand()) / (float)(RAND_MAX)) - objectsAreaSide / 2.0f;
	
	ventInstance->setTranslation(glm::vec3(x, y, z));
	ventInstance->scale(0.04f);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "Random Generation");
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

  Lotus::Camera camera;

  Lotus::Renderer renderer;
	renderer.startUp();

	renderer.setAmbientLight(glm::vec3(0.1, 0.1, 0.1));
	createDirectionalLight(renderer);

  ventMesh = meshManager.loadMesh(Lotus::assetPath("models/air_conditioner/AirConditioner.obj").string());

	for (int i = 0; i < objectsCount; i++)
	{
		createVent(renderer);
	}
	
	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		double currentTime = glfwGetTime();
		double dt = currentTime - lastTime;
		lastTime = currentTime;
		
		updateFromInputs(window, dt, &camera);

		LOTUS_LOG_INFO("FPS: {0}", 1 / dt);

		renderer.render(camera);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}