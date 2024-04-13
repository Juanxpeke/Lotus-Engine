#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
  Transform(
      const glm::vec3& translation = glm::vec3(0.0f),
      const glm::fquat& rotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      const glm::vec3& scale = glm::vec3(1.0f)) :
    localTranslation(translation),
    localRotation(rotation),
    localScale(scale) {}

  const glm::vec3& getLocalTranslation() const
  {
    return localTranslation;
  }

  const glm::fquat& getLocalRotation() const
  {
    return localRotation;
  }

  const glm::vec3& getLocalScale() const
  {
    return localScale;
  }

  glm::mat4 getModelMatrix() const
  {
    const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), localTranslation);
    const glm::mat4 rotationMatrix = glm::toMat4(localRotation);
    const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), localScale);

    return translationMatrix * rotationMatrix * scaleMatrix;
  }

  glm::mat4 getViewMatrix() const
  {
    const glm::vec3 up = getUpVector();
    const glm::vec3 front = getFrontVector();
    return glm::lookAt(localTranslation, localTranslation + front, up);
  }

  void translate(glm::vec3 translation) {
    localTranslation += translation;
  }

  void setTranslation(const glm::vec3 translation) {
    localTranslation = translation;
  }

  void scale(glm::vec3 scale){
    localScale *= scale;
  }

  void setScale(const glm::vec3& scale) {
    localScale = scale;
  }
  
  void rotate(glm::vec3 axis, float angle){
    localRotation = glm::rotate(localRotation, angle, axis);
  }

  void setRotation(const glm::fquat& rotation) {
    localRotation = rotation;
  }

  glm::vec3 getUpVector() const {
    return glm::rotate(localRotation, glm::vec3(0.0f, 1.0f, 0.0f));
  }
  
  glm::vec3 getRightVector() const {
    return glm::rotate(localRotation, glm::vec3(1.0f, 0.0f, 0.0f));
  }

  glm::vec3 getFrontVector() const {
    return glm::rotate(localRotation, glm::vec3(0.0f, 0.0f, -1.0f));
  }

private:
  glm::vec3 localTranslation;
  glm::fquat localRotation;
  glm::vec3 localScale;
};