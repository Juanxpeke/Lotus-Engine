#pragma once

#include <filesystem>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>
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

    void enable()
    {
      enabled = true;
    }

    void setFrameHistoryMaxSize(unsigned int size)
    {
      if (!enabled)
      {
        LOTUS_LOG_WARN("[Profiler Warning] Tried to set profiler history maximum size, but profiling is not enabled");
        return;
      }

      frameHistoryMaxSize = size;
    }

    void setAutomaticExport(bool value)
    {
      if (!enabled)
      {
        LOTUS_LOG_WARN("[Profiler Warning] Tried to set profiler automatic export, but profiling is not enabled");
        return;
      }

      exportAutomatically = value;
    }

    void setExportPath(std::string path)
    {
      if (!enabled)
      {
        LOTUS_LOG_WARN("[Profiler Warning] Tried to set profiler export path, but profiling is not enabled");
        return;
      }

      exportPath = path;
    }

    unsigned int getFrameHistoryMaxSize()
    {
      return frameHistoryMaxSize;
    }

    bool getExportAutomatically()
    {
      return exportAutomatically;
    }

    std::string getExportPath()
    {
      return exportPath;
    }

    void endFrame()
    {
      if (!enabled)
      {
        return;
      }
      
      bool timeRunning = false;

      for (bool running : currentFrameData.timesRunning)
      {
        timeRunning = timeRunning || running;
      }

      LOTUS_ASSERT(!timeRunning, "[Profiler Error] Ended frame with time running");

      while (frameHistory.size() >= frameHistoryMaxSize)
      {
        frameHistory.pop();
      }

      frameHistory.push(currentFrameData);

      currentFrameData = FrameData();

      frame++;

      if (frame >= exportAutomaticallyInitialFrame + frameHistoryMaxSize && exportAutomatically && !exportedAutomatically)
      {
        exportedAutomatically = true;
        exportFrameHistory();
      }
    }

    void startFrameTime(FrameTime frameTime)
    {
      if (!enabled)
      {
        return;
      }

      using std::chrono::microseconds;

      auto tStart = std::chrono::high_resolution_clock::now();

      LOTUS_ASSERT(!currentFrameData.timesRunning[static_cast<int>(frameTime)], "[Profiler Error] Tried to start already started time");

      currentFrameData.startTimes[static_cast<int>(frameTime)] = tStart;
      currentFrameData.timesRunning[static_cast<int>(frameTime)] = true;
    }
    
    void endFrameTime(FrameTime frameTime)
    {
      if (!enabled)
      {
        return;
      }

      using std::chrono::microseconds;

      auto tEnd = std::chrono::high_resolution_clock::now();

      LOTUS_ASSERT(currentFrameData.timesRunning[static_cast<int>(frameTime)], "[Profiler Error] Tried to end a time that hasn't been started");

      long time = std::chrono::duration_cast<microseconds>(tEnd - currentFrameData.startTimes[static_cast<int>(frameTime)]).count();

      currentFrameData.times[static_cast<int>(frameTime)] = time;
      currentFrameData.timesRunning[static_cast<int>(frameTime)] = false;
    }

    void increaseCounter(FrameCounter frameCounter)
    {
      if (!enabled)
      {
        return;
      }

      currentFrameData.counters[static_cast<int>(frameCounter)] += 1;
    }

    void exportFrameHistory()
    {
      if (!enabled)
      {
        LOTUS_LOG_WARN("[Profiler Warning] Tried to export history, but profiling is not enabled");
        return;
      }

      std::ofstream file(exportPath);

      std::queue<FrameData> frameHistoryCopy = frameHistory;
      std::vector<FrameData> frameHistoryIterator;

      frameHistoryIterator.reserve(frameHistoryCopy.size());

      while (!frameHistoryCopy.empty())
      {
        frameHistoryIterator.push_back(frameHistoryCopy.front());
        frameHistoryCopy.pop();
      }

      if (frameHistory.empty())
      {
        LOTUS_LOG_WARN("[Profiler Error] Tried to export empty history");
        return;
      }

      for (const FrameData& frameData : frameHistoryIterator)
      {
        if (file.is_open())
        {
          file << frameData.CSV() << std::endl;
        }
        else
        {
          LOTUS_LOG_ERROR("[Profiler Error] Can't open file {0}", exportPath);
        }
      }

      file.close();

      LOTUS_LOG_INFO("[Profiler Log] Exported history to {0}", exportPath);
    }

  private:

    Profiler() :  
      enabled(false),
      frameHistoryMaxSize(1000),
      frame(0),
      exportAutomatically(false),
      exportedAutomatically(false),
      exportAutomaticallyInitialFrame(0)
    {
      std::filesystem::path filePath = experimentPath("results/default.csv");
      exportPath = filePath.string();
    }

    bool enabled;

    unsigned int frame;
    FrameData currentFrameData;

    unsigned int frameHistoryMaxSize;
    std::queue<FrameData> frameHistory;

    bool exportAutomatically;
    bool exportedAutomatically;
    unsigned int exportAutomaticallyInitialFrame;
    std::string exportPath;
  };

}

#if NPROFILE
  #define LOTUS_ENABLE_PROFILING()                           (void(0))
  #define LOTUS_SET_PROFILER_FRAME_HISTORY_MAX_SIZE(size)    (void(0))
  #define LOTUS_SET_PROFILER_EXPORT_AUTOMATIC(value)         (void(0))
  #define LOTUS_SET_PROFILER_EXPORT_PATH(path)               (void(0))
  #define LOTUS_GET_PROFILER_FRAME_HISTORY_MAX_SIZE(size)    (void(0))
  #define LOTUS_GET_PROFILER_EXPORT_AUTOMATIC(value)         (void(0))
  #define LOTUS_GET_PROFILER_EXPORT_PATH()                   (void(0))
  #define LOTUS_EXPORT_PROFILER_HISTORY()                    (void(0))
  #define LOTUS_PROFILE_START_TIME(frameTime)                (void(0))
  #define LOTUS_PROFILE_END_TIME(frameTime)                  (void(0))
  #define LOTUS_PROFILE_INCREASE_COUNTER(frameCounter)       (void(0))
  #define LOTUS_PROFILE_END_FRAME()                          (void(0))
#else
  #define LOTUS_ENABLE_PROFILING()                           ::Lotus::Profiler::getProfiler().enable()
  #define LOTUS_SET_PROFILER_FRAME_HISTORY_MAX_SIZE(size)    ::Lotus::Profiler::getProfiler().setFrameHistoryMaxSize(size)
  #define LOTUS_SET_PROFILER_EXPORT_AUTOMATIC(value)         ::Lotus::Profiler::getProfiler().setAutomaticExport(value)
  #define LOTUS_SET_PROFILER_EXPORT_PATH(path)               ::Lotus::Profiler::getProfiler().setExportPath(path)
  #define LOTUS_GET_PROFILER_FRAME_HISTORY_MAX_SIZE()        ::Lotus::Profiler::getProfiler().getFrameHistoryMaxSize()
  #define LOTUS_GET_PROFILER_EXPORT_AUTOMATIC()              ::Lotus::Profiler::getProfiler().getExportAutomatically()
  #define LOTUS_GET_PROFILER_EXPORT_PATH()                   ::Lotus::Profiler::getProfiler().getExportPath()
  #define LOTUS_EXPORT_PROFILER_HISTORY()                    ::Lotus::Profiler::getProfiler().exportHistory()
  #define LOTUS_PROFILE_START_TIME(frameTime)                ::Lotus::Profiler::getProfiler().startFrameTime(frameTime)
  #define LOTUS_PROFILE_END_TIME(frameTime)                  ::Lotus::Profiler::getProfiler().endFrameTime(frameTime)
  #define LOTUS_PROFILE_INCREASE_COUNTER(frameCounter)       ::Lotus::Profiler::getProfiler().increaseCounter(frameCounter)
  #define LOTUS_PROFILE_END_FRAME()                          ::Lotus::Profiler::getProfiler().endFrame()
#endif