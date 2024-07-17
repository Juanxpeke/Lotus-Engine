#pragma once

#include <filesystem>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include "log.h"
#include "path_manager.h"

namespace Lotus
{

  enum class FrameTime
  {
    TraditionalSceneRenderTime,
    IndirectObjectBatchBuildTime,
    IndirectDrawBatchBuildTime,
    IndirectShaderBatchBuildTime,
    IndirectObjectBufferRefreshTime,
    IndirectObjectHandleBufferRefreshTime,
    IndirectMaterialBufferRefreshTime,
    IndirectIndirectBufferRefreshTime,
    IndirectSceneRenderTime,
    DataGenerationTime,
    TerrainRenderTime,
    ObjectsPlacesGenerationTime,
    FrameTimeCount
  };

  enum class FrameCounter
  {
    AddedTraditionalObjects,
    AddedIndirectObjects,
    ChunksLoaded,
    ObjectPlacesGenerated,
    FrameCounterCount
  };

  struct FrameData
  {
    unsigned int addedTraditionalObjects = 0;
    unsigned int addedIndirectObjects = 0;

    std::array<std::chrono::steady_clock::time_point, static_cast<int>(FrameTime::FrameTimeCount)> startTimes;
    std::array<long, static_cast<int>(FrameTime::FrameTimeCount)> times;
    std::array<bool, static_cast<int>(FrameTime::FrameTimeCount)> timesRunning;

    std::array<int, static_cast<int>(FrameCounter::FrameCounterCount)> counters;

    FrameData()
    {
      times.fill(0);
      timesRunning.fill(false);
      counters.fill(0);
    }

    std::string CSV() const
    {
      std::string csv;

      for (int i = 0; i < times.size(); i++)
      {
        csv = csv + std::to_string(times[i]);

        if (i != times.size() - 1)
        {
          csv += ",";
        }
      }

      if (!times.empty() && !counters.empty())
      {
        csv += ",";
      }

      for (int i = 0; i < counters.size(); i++)
      {
        csv = csv + std::to_string(counters[i]);

        if (i != counters.size() - 1)
        {
          csv += ",";
        }
      }

      return csv;
    }
  };

  class Profiler
  {
  public:

    static Profiler& getProfiler() noexcept
    { 
      static Profiler profiler;
      return profiler;
    }

    void setProfilerAppName(std::string name)
    {
      appName = name;
    }

    void endFrame()
    {
      if (dataExported) return;
      
      bool timeRunning = false;

      for (bool running : currentFrameData.timesRunning)
      {
        timeRunning = timeRunning || running;
      }

      LOTUS_ASSERT(!timeRunning, "[Profiler Error] Ended frame with time running");

      data.push_back(currentFrameData);

      currentFrameData = FrameData();

      frame++;

      if (frame >= maxFrameCount && !dataExported)
      {
        exportData();
      }
    }

    void startFrameTime(FrameTime frameTime)
    {
      if (dataExported) return;

      using std::chrono::microseconds;

      auto tStart = std::chrono::high_resolution_clock::now();

      LOTUS_ASSERT(!currentFrameData.timesRunning[static_cast<int>(frameTime)], "[Profiler Error] Tried to start already started time");

      currentFrameData.startTimes[static_cast<int>(frameTime)] = tStart;
      currentFrameData.timesRunning[static_cast<int>(frameTime)] = true;
    }
    
    void endFrameTime(FrameTime frameTime)
    {
      if (dataExported) return;

      using std::chrono::microseconds;

      auto tEnd = std::chrono::high_resolution_clock::now();

      LOTUS_ASSERT(currentFrameData.timesRunning[static_cast<int>(frameTime)], "[Profiler Error] Tried to end a time that hasn't been started");

      long time = std::chrono::duration_cast<microseconds>(tEnd - currentFrameData.startTimes[static_cast<int>(frameTime)]).count();

      currentFrameData.times[static_cast<int>(frameTime)] = time;
      currentFrameData.timesRunning[static_cast<int>(frameTime)] = false;
    }

    void increaseCounter(FrameCounter frameCounter)
    {
      if (dataExported) return;

      currentFrameData.counters[static_cast<int>(frameCounter)] += 1;
    }

  private:
    static constexpr unsigned int maxFrameCount = 1000;

    Profiler() : frame(0), appName("default"), dataExported(false)
    {

    }

    void exportData()
    {
      std::filesystem::path filePath = testPath(appName + ".csv");
      std::ofstream file(filePath.string());

      for (const FrameData& frameData : data)
      {
        if (file.is_open())
        {
          file << frameData.CSV() << std::endl;
        }
        else
        {
          LOTUS_LOG_ERROR("[Profiler Error] Can't open file {0}", filePath.string());
        }
      }

      file.close();

      dataExported = true;

      LOTUS_LOG_INFO("[Profiler Info] Exported data to {0}", filePath.string());
    }

    unsigned int frame;
    FrameData currentFrameData;

    std::string appName;
    std::vector<FrameData> data;
    bool dataExported;
  };

}

#if NPROFILE
  #define LOTUS_SET_PROFILER_APP(appName)              (void(0))
  #define LOTUS_PROFILE_START_TIME(frameTime)          (void(0))
  #define LOTUS_PROFILE_END_TIME(frameTime)            (void(0))
  #define LOTUS_PROFILE_INCREASE_COUNTER(frameCounter) (void(0))
  #define LOTUS_PROFILE_END_FRAME()                    (void(0))
#else
  #define LOTUS_SET_PROFILER_APP(appName)              ::Lotus::Profiler::getProfiler().setProfilerAppName(appName)
  #define LOTUS_PROFILE_START_TIME(frameTime)          ::Lotus::Profiler::getProfiler().startFrameTime(frameTime)
  #define LOTUS_PROFILE_END_TIME(frameTime)            ::Lotus::Profiler::getProfiler().endFrameTime(frameTime)
  #define LOTUS_PROFILE_INCREASE_COUNTER(frameCounter) ::Lotus::Profiler::getProfiler().increaseCounter(frameCounter)
  #define LOTUS_PROFILE_END_FRAME()                    ::Lotus::Profiler::getProfiler().endFrame()
#endif