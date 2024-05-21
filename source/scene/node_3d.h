#pragma once

#include "transform.h"

namespace Lotus
{
  class Node3D
  {
  public:

    Node3D() = default;

    Transform getTransform() const
    {
      return transform;
    }

    const glm::vec3& getLocalTranslation() const
    {
      return transform.getLocalTranslation();
    }

    const glm::fquat& getLocalRotation() const
    {
      return transform.getLocalRotation();
    }

    const glm::vec3& getLocalScale() const
    {
      return transform.getLocalScale();
    }

    glm::mat4 getModelMatrix() const
    {
      return transform.getModelMatrix();
    }

    glm::mat4 getViewMatrix() const
    {
      return transform.getViewMatrix();
    }

    void setTransform(const Transform& newTransform)
    {
      transform = newTransform;
    }

    void translate(glm::vec3 translation)
    {
      transform.translate(translation);
    }

    void setTranslation(const glm::vec3 translation)
    {
      transform.setTranslation(translation);
    }

    void scale(float scale)
    {
      transform.scale(scale);
    }

    void scale(glm::vec3 scale)
    {
      transform.scale(scale);
    }

    void setScale(const glm::vec3& scale)
    {
      transform.setScale(scale);
    }
    
    void rotate(glm::vec3 axis, float angle)
    {
      transform.rotate(axis, angle);
    }

    void setRotation(const glm::fquat& rotation)
    {
      transform.setRotation(rotation);
    }

    glm::vec3 getUpVector() const
    {
      return transform.getUpVector();
    }
    
    glm::vec3 getRightVector() const
    {
      return transform.getRightVector();
    }

    glm::vec3 getFrontVector() const
    {
      return transform.getFrontVector();
    }

  protected:					
    Transform transform;
  };
}
