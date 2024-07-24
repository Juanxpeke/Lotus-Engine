#include "lotus_engine.h"

class ExperimentApplication : public Lotus::RenderingApplication
{
public:
  ExperimentApplication(const std::string& experimentName) : Lotus::RenderingApplication(experimentName, 720, 720)
  {
    LOTUS_ENABLE_PROFILING();

    LOTUS_LOG_INFO("Object Rendering Method? Press 0 for traditional, 1 for indirect");

    bool objectRenderingMethodDecided = false;

    while(!objectRenderingMethodDecided)
    {
      glfwPollEvents();

      if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
      {
        objectRenderingMethod = Lotus::RenderingMethod::Traditional;
        objectRenderingMethodDecided = true;
      }
      else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
      {
        objectRenderingMethod = Lotus::RenderingMethod::Indirect;
        objectRenderingMethodDecided = true;
      }
    }

    std::string objectMethodName = objectRenderingMethod == Lotus::RenderingMethod::Traditional ? "Traditional" : "Indirect";
    std::string profilerAppName = experimentName + "-" + objectMethodName;

    LOTUS_SET_PROFILER_APP(profilerAppName);
  }
  
protected:
  Lotus::RenderingMethod objectRenderingMethod;
};