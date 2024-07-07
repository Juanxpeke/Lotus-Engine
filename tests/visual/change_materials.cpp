#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/path_manager.h"
#include "scene/camera.h"
#include "terrain/terrain.h"
#include "terrain/object_placer.h"
#include "render/rendering_server.h"

#include "render/indirect/mesh_manager.h"

#include "../test_util.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 14.4f;
const float cameraAngularSpeed = 2.0f;

glm::vec3 newObjectPosition(0, 0, -10);
glm::vec3 newObjectPositionOffset(5, 0, 0);

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
std::shared_ptr<Lotus::Mesh> sphereMesh;

std::vector<std::shared_ptr<Lotus::MeshInstance>> objects;
std::vector<std::shared_ptr<Lotus::Material>> materials;
std::vector<int> materialIndices;

class ObjectMaterialChangeEvent : public LotusTest::Event
{
public:

  ObjectMaterialChangeEvent(double time, int index) : LotusTest::Event(time), objectIndex(index) {}

  virtual void execute() override
  {
    int materialIndex = materialIndices[objectIndex];
    int newMaterialIndex = (materialIndex + 1) % materials.size();

    objects[objectIndex]->setMaterial(materials[newMaterialIndex]);
    materialIndices[objectIndex] = newMaterialIndex;
  }

private:
  int objectIndex;
};

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

  directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::pi<float>() * 0.5f);
  directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::pi<float>() * 0.25f);
  directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
}

std::shared_ptr<Lotus::Material> createFlatMaterial(Lotus::RenderingServer& renderingServer, glm::vec3 color)
{
  std::shared_ptr<Lotus::DiffuseFlatMaterial> flatMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));

  flatMaterial->setDiffuseColor(color);

  return flatMaterial;
}

std::shared_ptr<Lotus::MeshInstance> createNewObject(Lotus::RenderingServer& renderingServer, std::shared_ptr<Lotus::Material> material)
{
	std::shared_ptr<Lotus::MeshInstance> object = renderingServer.createObject(sphereMesh, material);

	object->translate(newObjectPosition);

  newObjectPosition += newObjectPositionOffset;
  
  return object;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "Different Materials");
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL)
  {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

	Lotus::Camera camera;

	Lotus::RenderingServer renderingServer;
	renderingServer.startUp();

	renderingServer.setAmbientLight(glm::vec3(0.5, 0.5, 0.5));
	createDirectionalLight(renderingServer);

  sphereMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);

  std::shared_ptr<Lotus::Material> redFlatMaterial = createFlatMaterial(renderingServer, glm::vec3(1.0, 0.0, 0.0));
  std::shared_ptr<Lotus::Material> greenFlatMaterial = createFlatMaterial(renderingServer, glm::vec3(0.0, 1.0, 0.0));
  std::shared_ptr<Lotus::Material> blueFlatMaterial = createFlatMaterial(renderingServer, glm::vec3(0.0, 0.0, 1.0));

	materials.push_back(redFlatMaterial);
  materials.push_back(greenFlatMaterial);
  materials.push_back(blueFlatMaterial);

	objects.push_back(createNewObject(renderingServer, redFlatMaterial));
	objects.push_back(createNewObject(renderingServer, greenFlatMaterial));
	objects.push_back(createNewObject(renderingServer, blueFlatMaterial));

  materialIndices.push_back(0);
  materialIndices.push_back(1);
  materialIndices.push_back(2);

  LotusTest::EventTimeline eventTimeline;
  ObjectMaterialChangeEvent e1(1.0, 0);
  ObjectMaterialChangeEvent e2(2.0, 1);
  ObjectMaterialChangeEvent e3(3.0, 2);
  eventTimeline.addEvent(&e1);
  eventTimeline.addEvent(&e2);
  eventTimeline.addEvent(&e3);
	
	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		double currentTime = glfwGetTime();
    double dt = currentTime - lastTime;
    lastTime = currentTime;

    updateFromInputs(window, dt, &camera);
    eventTimeline.update(dt);

		renderingServer.render(camera);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}