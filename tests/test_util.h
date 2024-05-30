#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "render/indirect/mesh_instance.h"

namespace LotusTest
{
  class Event
  {
  public:
    
    Event(double time) : triggerTime(time) {}

    virtual void execute() {};

    double getTriggerTime()
    {
      return triggerTime;
    }

    bool operator<(const Event& other) const
    {
      return triggerTime < other.triggerTime;
    }

  private:
    double triggerTime;
  };

  class EventTimeline
  {
  public:

    EventTimeline(double initialDuration = 0.0) :
      currentTime(0.0),
      duration(initialDuration),
      currentEventIndex(0) {}

    void update(double dt)
    {
      currentTime += dt;

      if ((currentTime - dt) <= duration && !events.empty())
      {
        if (currentTime >= events[currentEventIndex]->getTriggerTime())
        {
          events[currentEventIndex]->execute();
          currentEventIndex = (currentEventIndex + 1) % events.size();
        }
      }
      else if (currentTime > duration)
      {
        currentTime = 0.0;
        currentEventIndex = 0;
      }
    }

    void addEvent(Event* event)
    {
      if (event->getTriggerTime() > duration)
      {
        duration = event->getTriggerTime();
      }

      events.push_back(event);

      std::sort(events.begin(), events.end());
    }

  private:
    double currentTime;
    double duration;
    int currentEventIndex;
    std::vector<Event*> events;
  };

  class TransformChange
  {
  public:
    TransformChange(std::shared_ptr<Lotus::MeshInstance> object) : objectPtr(object) {}

    virtual void apply(double dt) = 0;

  protected:
    std::shared_ptr<Lotus::MeshInstance> objectPtr;
  };

  class CyclicDirectedTranslation : public TransformChange
  {
  public:
    CyclicDirectedTranslation(
        std::shared_ptr<Lotus::MeshInstance> object,
        glm::vec3 initialVelocity = glm::vec3(0.0f, 2.0f, 0.0f),
        double initialDistance = 2.0) :
      TransformChange(object),
      velocity(initialVelocity),
      distance(initialDistance)
    {
      basePosition = object->getLocalTranslation();
    }

    virtual void apply(double dt) override
    {
      if (glm::distance(basePosition, objectPtr->getLocalTranslation()) > distance)
      {
        basePosition = basePosition + glm::normalize(velocity) * static_cast<float>(distance);
        objectPtr->setTranslation(basePosition);
        velocity = -velocity;
      }
      else
      {
        glm::vec3 translation = velocity * static_cast<float>(dt);

        objectPtr->translate(translation);
      }
    }

  private:
    glm::vec3 basePosition;
    glm::vec3 velocity;
    double distance;
  };

}

