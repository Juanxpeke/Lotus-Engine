#include "rendering_application.h"

namespace Lotus
{
  
  RenderingApplication::RenderingApplication(const std::string& applicationName, int windowWidth, int windowHeight) :
    Application::Application(applicationName, windowWidth, windowHeight)
  {
    renderingServer.startUp();
  }

  void RenderingApplication::update(float deltaTime)
  {
    updateCamera(deltaTime);
  }

  void RenderingApplication::updateCamera(float deltaTime)
  {
    // Translation
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
      camera.translate(camera.getFrontVector() * deltaTime * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
      camera.translate(camera.getRightVector() * deltaTime * -cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
      camera.translate(camera.getFrontVector() * deltaTime * -cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
      camera.translate(camera.getRightVector() * deltaTime * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
      camera.translate(glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
      camera.translate(glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime * -cameraSpeed);
    }
    // Rotation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
      camera.rotate(camera.getRightVector(), deltaTime * cameraAngularSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
      camera.rotate(glm::vec3(0.0f, 1.0f, 0.0f), deltaTime * cameraAngularSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
      camera.rotate(camera.getRightVector(), deltaTime * -cameraAngularSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
      camera.rotate(glm::vec3(0.0f, 1.0f, 0.0f), deltaTime * -cameraAngularSpeed);
    }
  }

  void RenderingApplication::render()
  {
    renderingServer.render(camera);
  }
}