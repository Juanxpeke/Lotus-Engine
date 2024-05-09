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

DirectionalLight* Renderer::createDirectionalLight()
{
  directionalLights.emplace_back();
  return &directionalLights.back();
}

PointLight* Renderer::createPointLight()
{
  pointLights.emplace_back();
  return &pointLights.back();
}