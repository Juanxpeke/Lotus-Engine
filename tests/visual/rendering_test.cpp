#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/path_manager.h"
#include "scene/camera.h"
#include "lighting/light_manager.h"
#include "terrain/terrain.h"
#include "terrain/object_placer.h"
#include "render/indirect/indirect_scene.h"
#include "render/rendering_server.h"

#include "render/indirect/mesh_manager.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 128.0f;
const float cameraAngularSpeed = 2.0f;

const int objectsCount = 32000;
const float objectsAreaSide = 160.f;

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
  
  Lotus::PerlinNoiseConfig perlinConfiguration;
  std::shared_ptr<Lotus::ProceduralDataGenerator> dataGenerator = std::make_shared<Lotus::ProceduralDataGenerator>(512, 6, perlinConfiguration);
  
  std::shared_ptr<Lotus::LightManager> lightManager  = std::make_shared<Lotus::LightManager>();
  std::shared_ptr<Lotus::IndirectScene> indirectScene = std::make_shared<Lotus::IndirectScene>();
  std::shared_ptr<Lotus::Terrain> clipmap = std::make_shared<Lotus::Terrain>(dataGenerator);

  Lotus::RenderingServer renderingServer(lightManager, indirectScene, clipmap);

  Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();

  std::shared_ptr<Lotus::Mesh> cubeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Cube);
  std::shared_ptr<Lotus::Mesh> sphereMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);

  std::shared_ptr<Lotus::DiffuseFlatMaterial> redMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(indirectScene->createMaterial(Lotus::MaterialType::DiffuseFlat));
  redMaterial->setDiffuseColor({ 1.0, 0.0, 0.0 });
  std::shared_ptr<Lotus::DiffuseFlatMaterial> blueMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(indirectScene->createMaterial(Lotus::MaterialType::DiffuseFlat));
  blueMaterial->setDiffuseColor({ 0.0, 0.0, 1.0 });

  Lotus::ObjectPlacer objectPlacer(dataGenerator, indirectScene, 36.0);

  objectPlacer.addObject(cubeMesh, redMaterial, 30.0);
  objectPlacer.addObject(sphereMesh, blueMaterial, 30.0);


  objectPlacer.generateAllObjects();

  lightManager->setAmbientLight({ 0.1, 0.1, 0.1 });

  std::shared_ptr<Lotus::DirectionalLight> directionalLight = lightManager->createDirectionalLight();
  directionalLight->setLightColor({ 0.2, 0.1, 0.01 });
  directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::pi<float>() * 0.5f);
  directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::pi<float>() * 0.25f);

  std::shared_ptr<Lotus::PointLight> pointLight = lightManager->createPointLight();
  pointLight->setLightColor({ 1.0, 1.0, 1.0 });
  pointLight->setLightRadius(80.0);
  pointLight->setLightIntensity(200.0);

  pointLight->translate({ 0, 75, 0 });

	renderingServer.startUp();

	//lightManager->setAmbientLight(glm::vec3(0.1, 0.1, 0.1));
	
	double lastTime = glfwGetTime();

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