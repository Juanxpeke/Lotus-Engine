#pragma once

#include "application.h"
#include "render/rendering_server.h"

namespace Lotus
{

  class RenderingApplication : public Application
  {
  public:
    
    RenderingApplication(const std::string& applicationName, int windowWidth, int windowHeight);

  protected:

    virtual void update(float deltaTime) override;
    virtual void render() override;
    
    Camera camera;
    float cameraSpeed = 32.0f;
    float cameraAngularSpeed = 2.0f;
    
    RenderingServer renderingServer;

  private:

    void updateCamera(float deltaTime);

  };

}