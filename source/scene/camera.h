#pragma once

#include "node_3d.h"

namespace Lotus
{
  class Camera : public Node3D
  {
  public:

    Camera(
        float fov = 70.0f, 
        float ratio = 1.0f,
        float nearPlane = 0.1f,
        float farPlane = 1000.0f) :
      fieldOfView(fov),
      aspectRatio(ratio),
      zNearPlane(nearPlane),
      zFarPlane(farPlane)
    {}

    glm::mat4 getProjectionMatrix() const
    {
      return glm::perspective(glm::radians(fieldOfView), aspectRatio, zNearPlane, zFarPlane);
    }

    float getFieldOfView() const { return fieldOfView;}
    float getZNearPlane()  const { return zNearPlane; }
    float getZFarPlane()   const { return zFarPlane;  }
    float getAspectRatio() const { return aspectRatio;}

    void setFieldOfView(float value) { fieldOfView = value; }
    void setZNearPlane(float value)  { zNearPlane  = value; }
    void setZFarPlane(float value)   { zFarPlane   = value; }
    void setAspectRatio(float value) { aspectRatio = value; }

  private:					
    float fieldOfView = 70.0f;
    float aspectRatio = 1.0f;
    float zNearPlane = 0.1f;
    float zFarPlane = 1000.0f;
  };
}
