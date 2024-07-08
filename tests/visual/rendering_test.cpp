#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lotus_engine.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 128.0f;
const float cameraAngularSpeed = 2.0f;

void updateFromInputs(GLFWwindow* window, float dt, Lotus::RenderingServer* renderingServerPtr, Lotus::Camera* cameraPtr)
{
  // Mode
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
    renderingServerPtr->switchRenderingMode();
  }

  // Camera Translation
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

  // Camera Rotation
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

  sprintf(title, "Rendering Test");
	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (window == nullptr)
  {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

  Lotus::Camera camera;
  
  Lotus::PerlinNoiseConfig noiseConfiguration;
  std::shared_ptr<Lotus::ProceduralDataGenerator> dataGenerator = std::make_shared<Lotus::ProceduralDataGenerator>(512, 6, noiseConfiguration);

  Lotus::RenderingServer renderingServer;

  Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();

  std::shared_ptr<Lotus::Mesh> cubeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Cube);
  std::shared_ptr<Lotus::Mesh> sphereMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);

  std::shared_ptr<Lotus::DiffuseFlatMaterial> redMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
  std::shared_ptr<Lotus::DiffuseFlatMaterial> blueMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
  
  redMaterial->setDiffuseColor({ 1.0, 0.0, 0.0 });
  blueMaterial->setDiffuseColor({ 0.0, 0.0, 1.0 });

  Lotus::ObjectPlacer objectPlacer(dataGenerator, &renderingServer, 36.0);

  objectPlacer.addObject(cubeMesh, redMaterial, 30.0);
  objectPlacer.addObject(sphereMesh, blueMaterial, 30.0);
  objectPlacer.initialize();

  renderingServer.setAmbientLight({ 0.1, 0.1, 0.1 });

  std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderingServer.createDirectionalLight();
  directionalLight->setLightColor({ 0.2, 0.1, 0.01 });
  directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
  directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f));

  std::shared_ptr<Lotus::PointLight> pointLight = renderingServer.createPointLight();
  pointLight->translate({ 0, 75, 0 });
  pointLight->setLightColor({ 1.0, 1.0, 1.0 });
  pointLight->setLightRadius(80.0);
  pointLight->setLightIntensity(200.0);

  Lotus::Terrain terrain(dataGenerator);
  renderingServer.setTerrain(&terrain);

	renderingServer.startUp();
	
	double lastTime = glfwGetTime();

  glm::vec3 cameraPosition = camera.getLocalTranslation();
	
  while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		double currentTime = glfwGetTime();
		double dt = currentTime - lastTime;
		lastTime = currentTime;
		
		updateFromInputs(window, dt, &renderingServer, &camera);

    cameraPosition = camera.getLocalTranslation();

    dataGenerator->registerObserverPosition(glm::vec2(cameraPosition.x, cameraPosition.z));

    objectPlacer.update();
		
    renderingServer.render(camera);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}