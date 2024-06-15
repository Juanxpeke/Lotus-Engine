#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Lotus {

  class Log
  {
  public:

    static std::shared_ptr<spdlog::logger>& getLogger() noexcept
    { 
      static Log log;
      return log.sLogger;
    }

  private:
    Log()
    {
      spdlog::set_pattern("%^[%T] %n: %v%$");
      sLogger = spdlog::stdout_color_mt("Lotus");
    }

    Log(const Log& log) = delete;

    Log& operator=(const Log& other) = delete;
    
    std::shared_ptr<spdlog::logger> sLogger;
  };
}

#if NDEBUG
  #define LOTUS_LOG_INFO(...)     (void(0))
  #define LOTUS_LOG_ERROR(...)    (void(0))
  #define LOTUS_ASSERT(expr, ...) (void(0))
#else
  #define LOTUS_LOG_INFO(...)   ::Lotus::Log::getLogger()->info(__VA_ARGS__)
  #define LOTUS_LOG_ERROR(...)  ::Lotus::Log::getLogger()->error(__VA_ARGS__)

	#ifndef WIN32
    #define LOTUS_ASSERT(expr, ...)  { if(!(expr)) { LOTUS_LOG_ERROR(__VA_ARGS__); assert(expr); } }
	#else
		#define LOTUS_ASSERT(expr, ...)  { if(!(expr)) { LOTUS_LOG_ERROR(__VA_ARGS__); __debugbreak(); } }
	#endif

#endif