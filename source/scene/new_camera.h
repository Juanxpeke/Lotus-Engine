#pragma once

#include "transform.h"

class Camera
{
public:

  Camera(
      float fov = 50.0f, 
      float ratio = 16.0f / 9.0f,
      float nearPlane = 0.1f,
      float farPlane = 100.0f) :
    fieldOfView(fov),
    aspectRatio(ratio),
    zNearPlane(nearPlane),
    zFarPlane(farPlane)
  {}

  Transform getTransform() const
  {
    return transform;
  }

  glm::mat4 getProjectionMatrix() const
  {
    return glm::perspective(glm::radians(fieldOfView), aspectRatio, zNearPlane, zFarPlane);
  }

  float getFieldOfView() const { return fieldOfView;}
  float getZNearPlane() const { return zNearPlane;}
  float getZFarPlane() const { return zFarPlane;}
  float getAspectRatio() const { return aspectRatio;}
  void setFieldOfView(float value) { fieldOfView = value; }
  void setZNearPlane(float value) { zNearPlane = value; }
  void setZFarPlane(float value) { zFarPlane = value; }
  void setAspectRatio(float value) { aspectRatio = value; }

private:					
  Transform transform;

  float fieldOfView = 50.0f;
  float zNearPlane = 0.1f;
  float zFarPlane = 100.0f;
  float aspectRatio = 16.0f / 9.0f;
};
