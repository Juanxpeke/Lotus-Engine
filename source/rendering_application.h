#pragma once

#include "application.h"
#include "render/rendering_server.h"

namespace Lotus
{

  class RenderingApplication : public Application
  {
  public:
    
    RenderingApplication(const std::string& applicationName, int windowWidth, int windowHeight);

    Camera camera;
    float cameraSpeed = 14.4f;
    float cameraAngularSpeed = 2.0f;
    
    RenderingServer renderingServer;

  private:
    virtual void update(float deltaTime) override;
    void updateCamera(float deltaTime);
  };

}