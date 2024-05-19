#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Lotus
{
  class Window
  {
  public:

    Window();
    Window(const Window& window) = delete;

    ~Window();

    Window& operator=(const Window& window) = delete;

    bool shouldClose() const noexcept;

    bool isFullScreen() const noexcept;
    glm::ivec2 getWindowDimensions() const noexcept;
    glm::ivec2 getWindowFrameBufferSize() const noexcept;

    void setFullScreen(bool value) noexcept;
    void setWindowDimensions(const glm::ivec2 &dimensions) noexcept;
    void setSwapInterval(int interval) noexcept;

  private:

    void startUp() noexcept;
    void shutDown() noexcept;
    void update() noexcept;

    class WindowImplementation;
    std::unique_ptr<WindowImplementation> pImpl;
  };
}