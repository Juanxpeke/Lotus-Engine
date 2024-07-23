#pragma once

#define GLFW_INCLUDE_NONE

#include <string>
#include "util/log.h"
#include "util/window_entry.h"

namespace Lotus
{

  class Application
  {
  public:

    Application(const std::string& applicationName, int windowWidth, int windowHeight);
    ~Application();

    void run();

    GLFWwindow* window;

  protected:
    virtual void update(float deltaTime);

    std::string name;
    int width;
    int height;

  };

}