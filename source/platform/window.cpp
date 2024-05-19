#include "window.h"

#include <string>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#ifndef _WINDOWS_
#undef APIENTRY
#endif

namespace Lotus
{
  class Window::WindowImplementation
  {
  public:
    WindowImplementation() = default;

    WindowImplementation(const WindowImplementation& window) = delete;

    WindowImplementation& operator=(const WindowImplementation& window) = delete;

    void startUp() noexcept
    {
      // ASSERT(windowPtr == nullptr, "Calling Window::StartUp for the second time!!!.");
      const int success = glfwInit();
      // ASSERT(success, "Could not initialize GLFW!");

      std::string windowTitle = "Lotus Engine";
      int windowWidth = 640;
      int windowHeight = 640;
      bool fullScreen = false;

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      #if __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
      #endif
      #ifndef NDEBUG
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
      #endif
      auto monitor = glfwGetPrimaryMonitor();
      
      windowPtr = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), fullScreen ? monitor : NULL, NULL);
      
      glfwMakeContextCurrent(windowPtr);

      glfwGetWindowPos(windowPtr, &oldWindowPos[0], &oldWindowPos[1]);
      
      int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
      // ASSERT(status, "Failed to initialize Glad!");

      glfwSwapInterval(1);
    }

    void shutDown() noexcept
    {
      //ASSERT(m_windowHandle != nullptr, "Calling Window::ShutDown for the second time or without calling Window::Startup first.");
      glfwDestroyWindow(windowPtr);
    }

    void update() noexcept
    {
      glfwSwapBuffers(windowPtr);		
    }

    bool shouldClose() const noexcept
    {
      return glfwWindowShouldClose(windowPtr);
    }

    bool isFullScreen() const noexcept
    {
      return glfwGetWindowMonitor(windowPtr) != NULL;
    }

    glm::ivec2 getWindowDimensions() const noexcept
    {
      int width, height;
      glfwGetWindowSize(windowPtr, &width, &height);
      return glm::ivec2(width, height);
    }

    glm::ivec2 getWindowFrameBufferSize() const noexcept
    {
      int width, height;
      glfwGetFramebufferSize(windowPtr, &width, &height);
      return glm::ivec2(width, height);
    }

    void setFullScreen(bool value) noexcept
    {
      if (value == isFullScreen()) { return; }
      
      if (value)
      {
        glfwGetWindowPos(windowPtr, &oldWindowPos[0], &oldWindowPos[1]);
        auto monitor = glfwGetPrimaryMonitor();
        int width, height;
        glfwGetWindowSize(windowPtr, &width, &height);
        glfwSetWindowMonitor(windowPtr, monitor, 0, 0, width, height, GLFW_DONT_CARE);
      }
      else
      {
        int width, height;
        glfwGetWindowSize(windowPtr, &width, &height);
        glfwSetWindowMonitor(windowPtr, NULL, oldWindowPos.x, oldWindowPos.y, width, height, GLFW_DONT_CARE);
      }
    }

    void setWindowDimensions(const glm::ivec2 &dimensions) noexcept
    {
      glfwSetWindowSize(windowPtr, dimensions.x, dimensions.y);
    }

    void setSwapInterval(int interval) noexcept
    {
      glfwSwapInterval(interval);
    }

  private:
    GLFWwindow* windowPtr = nullptr;
    glm::ivec2 oldWindowPos = glm::vec2(0,0);
  };

  Window::Window() : pImpl(std::make_unique<WindowImplementation>()) {}

  Window::~Window() = default;

  void Window::startUp() noexcept
  {
    pImpl->startUp();
  }

  void Window::shutDown() noexcept
  {
    pImpl->shutDown();
  }

  void Window::update() noexcept
  {
    pImpl->update();
  }

  bool Window::shouldClose() const noexcept
  {
    return pImpl->shouldClose();
  }

  bool Window::isFullScreen() const noexcept
  {
    return pImpl->isFullScreen();
  }

  glm::ivec2 Window::getWindowDimensions() const noexcept
  {
    return pImpl->getWindowDimensions();
  }

  glm::ivec2 Window::getWindowFrameBufferSize() const noexcept
  {
    return pImpl->getWindowFrameBufferSize();
  }

  void Window::setFullScreen(bool value) noexcept
  {
    pImpl->setFullScreen(value);
  }

  void Window::setWindowDimensions(const glm::ivec2& dimensions) noexcept
  {
    pImpl->setWindowDimensions(dimensions);
  }

  void Window::setSwapInterval(int interval) noexcept
  {
    pImpl->setSwapInterval(interval);
  }

}