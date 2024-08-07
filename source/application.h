#pragma once

#define GLFW_INCLUDE_NONE

#include <string>
#include <imgui.h>
#include "math/types.h"
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

    virtual void update(float deltaTime) {}
    virtual void render() {}
    virtual void renderGUI() {}

    void setBackgroundColor(const glm::vec3& color);
    void disableVSync();

    std::string name;
    int width;
    int height;

    std::string vendor;
    std::string device;
  };

}