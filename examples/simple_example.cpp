#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "lotus_engine.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 14.4f;
const float cameraAngularSpeed = 2.0f;

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

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

void createDirectionalLight(Lotus::RenderingServer& renderingServer)
{
  std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderingServer.createDirectionalLight();

  directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
  directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f));
  directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
}

void createPointLights(Lotus::RenderingServer& renderingServer)
{
  std::shared_ptr<Lotus::PointLight> pointLight = renderingServer.createPointLight();

  pointLight->translate(glm::vec3(0.0f, 5.0f, 0.0f));
  pointLight->setLightColor(glm::vec3(1.0f, 1.0f, 1.0f));
  pointLight->setLightIntensity(25.f);
  pointLight->setLightRadius(400.f);
}

void createPlane(Lotus::RenderingServer& renderingServer)
{
  std::shared_ptr<Lotus::Mesh> planeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane);

  // auto& textureManager = Lotus::Traditional::TextureManager::getInstance();
  //std::shared_ptr<Lotus::Traditional::Texture> planeDiffuseTexture = textureManager.loadTexture(Lotus::assetPath("textures/wood.png"));

	std::shared_ptr<Lotus::DiffuseFlatMaterial> planeMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));

	//planeMaterial->setDiffuseTexture(planeDiffuseTexture);

	std::shared_ptr<Lotus::MeshObject> planeObject = renderingServer.createObject(planeMesh, planeMaterial, Lotus::RenderingMethod::Indirect);

  planeObject->rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f));
	planeObject->scale(20.f);
}

/*
void createVent(Lotus::RenderingServer& renderer)
{
	auto& meshManager = MeshManager::getInstance();
	std::shared_ptr<Mesh> ventMesh = meshManager.loadMesh(assetPath("models/air_conditioner/AirConditioner.obj"), true);

  auto& textureManager = TextureManager::getInstance();
  std::shared_ptr<Texture> ventDiffuseTexture = textureManager.loadTexture(assetPath("models/air_conditioner/Albedo.png"));

	std::shared_ptr<DiffuseTexturedMaterial> ventMaterial = std::static_pointer_cast<DiffuseTexturedMaterial>(renderer.createMaterial(MaterialType::DiffuseTextured));

	ventMaterial->setDiffuseTexture(ventDiffuseTexture);

	MeshInstance* ventInstance = renderer.createMeshInstance(ventMesh, ventMaterial);

  ventInstance->translate(glm::vec3(0.0f, 10.0f, -18.0f));
	ventInstance->scale(0.3f);
}
*/

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "Simple Example");
	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (window == NULL)
  {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

  Lotus::Camera camera;

  Lotus::RenderingServer renderingServer;
	renderingServer.startUp();

  renderingServer.setAmbientLight(glm::vec3(0.1, 0.1, 0.1));
  createDirectionalLight(renderingServer);
  createPointLights(renderingServer);

  createPlane(renderingServer);
	//createVent(renderingServer);
	
	double lastTime = glfwGetTime();

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		double currentTime = glfwGetTime();
    double dt = currentTime - lastTime;
    lastTime = currentTime;

    updateFromInputs(window, dt, &camera);

		renderingServer.render(camera);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}