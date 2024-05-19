#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../path_manager.h"

void Renderer::setAmbientLight(glm::vec3 color)
{
  ambientLight = color;
}

std::shared_ptr<DirectionalLight> Renderer::createDirectionalLight()
{
  std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
  directionalLights.push_back(directionalLight);

  return directionalLight;
}

std::shared_ptr<PointLight> Renderer::createPointLight()
{
  std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
  pointLights.push_back(pointLight);

  return pointLight;
}