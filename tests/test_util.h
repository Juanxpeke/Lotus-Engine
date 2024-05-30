#pragma once

#include <algorithm>
#include <vector>

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

}

