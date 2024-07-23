#include "application.h"

#include "util/opengl_entry.h"

namespace Lotus
{

  Application::Application(const std::string& applicationName, int windowWidth, int windowHeight) :
    name(applicationName),
    width(windowWidth),
    height(windowHeight)
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    if (window == nullptr)
    {
      glfwTerminate();
      LOTUS_ASSERT(false, "[Application Error] Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    if (!status)
    {
      glfwTerminate();
      LOTUS_ASSERT(false, "[Application Error] Failed to initialize Glad");
    }

    glViewport(0, 0, width, height);
  }

  Application::~Application()
  {
    glfwDestroyWindow(window);
	  glfwTerminate();
  }

  void Application::run()
  {
    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();

      float currentFrame = glfwGetTime();
      
      update(currentFrame - lastFrame);
      
      lastFrame = currentFrame;
      
      glfwSwapBuffers(window);
    }
  }
  
  void Application::update(float deltaTime) {}

}