#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "math/types.h"
#include "util/path_manager.h"
#include "scene/camera.h"
#include "render/indirect/renderer.h"
#include "render/mesh_manager.h"
#include "../test_util.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 14.4f;
const float cameraAngularSpeed = 2.0f;

glm::vec3 newObjectPosition(0, 0, -10);
glm::vec3 newObjectPositionOffset(5, 0, 0);

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
std::shared_ptr<Lotus::DiffuseFlatMaterial> whiteFlatMaterial; 

std::vector<std::shared_ptr<Lotus::MeshObject>> objects;
std::vector<std::shared_ptr<Lotus::Mesh>> meshes;
std::vector<int> meshIndices;

class ObjectMeshChangeEvent : public LotusTest::Event
{
public:

  ObjectMeshChangeEvent(double time, int index) : LotusTest::Event(time), objectIndex(index) {}

  virtual void execute() override
  {
    int meshIndex = meshIndices[objectIndex];
    int newMeshIndex = (meshIndex + 1) % meshes.size();

    objects[objectIndex]->setMesh(meshes[newMeshIndex]);
    meshIndices[objectIndex] = newMeshIndex;
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

void createDirectionalLight(Lotus::Renderer& renderer)
{
  std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderer.createDirectionalLight();

  directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::pi<float>() * 0.5f);
  directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::pi<float>() * 0.25f);
  directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
}

std::shared_ptr<Lotus::MeshInstance> createNewObject(Lotus::Renderer& renderer, std::shared_ptr<Lotus::Mesh> mesh)
{
	std::shared_ptr<Lotus::MeshInstance> object = renderer.createMeshInstance(mesh, whiteFlatMaterial);

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

  sprintf(title, "Change Meshes");
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

	Lotus::Renderer renderer;
	renderer.startUp();

	renderer.setAmbientLight(glm::vec3(0.5, 0.5, 0.5));
	createDirectionalLight(renderer);

  std::shared_ptr<Lotus::Mesh> planeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane);
  std::shared_ptr<Lotus::Mesh> cubeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Cube);
  std::shared_ptr<Lotus::Mesh> sphereMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);

  whiteFlatMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderer.createMaterial(Lotus::MaterialType::DiffuseFlat));

  meshes.push_back(planeMesh);
  meshes.push_back(cubeMesh);
  meshes.push_back(sphereMesh);

	objects.push_back(createNewObject(renderer, planeMesh));
	objects.push_back(createNewObject(renderer, cubeMesh));
	objects.push_back(createNewObject(renderer, sphereMesh));

  meshIndices.push_back(0);
  meshIndices.push_back(1);
  meshIndices.push_back(2);

  LotusTest::EventTimeline eventTimeline;
  ObjectMeshChangeEvent e1(1.0, 0);
  ObjectMeshChangeEvent e2(2.0, 1);
  ObjectMeshChangeEvent e3(3.0, 2);
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

		renderer.render(camera);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}