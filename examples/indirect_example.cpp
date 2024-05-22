#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "scene/camera.h"
#include "render/indirect/renderer.h"
#include "render/indirect/mesh_manager.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 14.4f;
const float cameraAngularSpeed = 2.0f;

const int objectsCount = 1000;
const float objectsAreaSide = 50.f;

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

  Lotus::Camera camera;

  Lotus::Renderer renderer;
  renderer.startUp();

  auto& meshManager = Lotus::MeshManager::getInstance();
  std::shared_ptr<Lotus::Mesh> cube = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Cube);
  std::shared_ptr<Lotus::Mesh> sphere = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);

  std::shared_ptr<Lotus::DiffuseFlatMaterial> material = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderer.createMaterial(Lotus::MaterialType::DiffuseFlat));
  //std::shared_ptr<Lotus::DiffuseFlatMaterial> material(0);
  material->setDiffuseColor(glm::vec3(1.0, 0.0, 1.0));

  std::shared_ptr<Lotus::MeshInstance> object1 = renderer.createMeshInstance(sphere, material);
  std::shared_ptr<Lotus::MeshInstance> object2 = renderer.createMeshInstance(sphere, material);
  
  object1->translate(glm::vec3(3, 0, 0));

	float lastTime = glfwGetTime();

  float meshChangeElapsedTime = 0.0;
  float meshChangeSphere = false;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		float currentTime = glfwGetTime();
		float dt = currentTime - lastTime;
		lastTime = currentTime;
    meshChangeElapsedTime += dt;

		updateFromInputs(window, dt, &camera);
    
    object1->translate(glm::vec3(dt, 0, 0));
	
    if (meshChangeElapsedTime > 3.0)
    {
      if (meshChangeSphere)
      {
        object1->setMesh(sphere);
      }
      else
      {
        object1->setMesh(cube);
      }

      meshChangeElapsedTime = 0.0;
      meshChangeSphere = !meshChangeSphere;
    }

		// std::cout << "FPS: " << 1.0f / dt << std::endl;

		renderer.render(camera);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}