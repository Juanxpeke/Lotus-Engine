#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "./scene/camera.h"
#include "./path_manager.h"
#include "instanced/i_mesh_manager.h"
#include "instanced/i_shader_program.h"
#include "instanced/i_texture_manager.h"
#include "instanced/i_diffuse_textured_material.h"
#include "instanced/i_diffuse_flat_material.h"
#include "instanced/i_mesh_instance.h"
#include "instanced/i_renderer.h"

int width = 720;
int height = 720;
char title[256];

const float cameraSpeed = 14.4f;
const float cameraAngularSpeed = 2.0f;

void updateFromInputs(GLFWwindow* window, float dt, Camera* cameraPtr)
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

void createDirectionalLight(Renderer& renderer)
{
  DirectionalLight* directionalLight = renderer.createDirectionalLight();

  directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::pi<float>() * 0.5f);
  directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::pi<float>() * 0.25f);
  directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
}

void createPointLights(Renderer& renderer)
{
  PointLight* pointLight = renderer.createPointLight();

  pointLight->translate(glm::vec3(0.0f, 5.0f, 0.0f));
  pointLight->setLightColor(glm::vec3(1.0f, 1.0f, 1.0f));
  pointLight->setLightIntensity(25.f);
  pointLight->setLightRadius(400.f);
}

void createPlane(Renderer& renderer)
{
  auto& meshManager = MeshManager::getInstance();
  std::shared_ptr<Mesh> planeMesh = meshManager.loadMesh(Mesh::PrimitiveType::Plane);

  auto& textureManager = TextureManager::getInstance();
  std::shared_ptr<Texture> planeDiffuseTexture = textureManager.loadTexture(assetPath("textures/wood.png"));

  std::shared_ptr<DiffuseTexturedMaterial> planeMaterial = std::static_pointer_cast<DiffuseTexturedMaterial>(renderer.createMaterial(MaterialType::DiffuseTextured));

  std::shared_ptr<DiffuseFlatMaterial> ventMaterial = std::static_pointer_cast<DiffuseFlatMaterial>(renderer.createMaterial(MaterialType::DiffuseFlat));

  ventMaterial->setDiffuseColor(glm::vec3(1.0f, 0.f, 0.f));

  planeMaterial->setDiffuseTexture(planeDiffuseTexture);

  MeshInstance* planeInstance = renderer.createMeshInstance(planeMesh, planeMaterial);

  planeInstance->rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f));
  planeInstance->scale(20.f);
}

void createVent(Renderer& renderer)
{
  auto& meshManager = MeshManager::getInstance();
  std::shared_ptr<Mesh> ventMesh = meshManager.loadMesh(assetPath("models/air_conditioner/AirConditioner.obj").string());

  std::shared_ptr<DiffuseFlatMaterial> ventMaterial = std::static_pointer_cast<DiffuseFlatMaterial>(renderer.createMaterial(MaterialType::DiffuseFlat));

  ventMaterial->setDiffuseColor(glm::vec3(1.0f, 0.f, 0.f));

  MeshInstance* ventInstance = renderer.createMeshInstance(ventMesh, ventMaterial);

  ventInstance->translate(glm::vec3(0.0f, 10.0f, -18.0f));
  ventInstance->scale(0.3f);
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

  renderer.setAmbientLight(glm::vec3(0.1, 0.1, 0.1));
  createDirectionalLight(renderer);
  createPointLights(renderer);

  createPlane(renderer);
  createVent(renderer);

  float lastTime = glfwGetTime();

  // Main while loop
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    
    float currentTime = glfwGetTime();
    float dt = currentTime - lastTime;
    lastTime = currentTime;

    updateFromInputs(window, dt, &camera);

    std::cout << "FPS: " << 1.0f / dt << std::endl;

    renderer.render(camera);

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}