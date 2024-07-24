#include "lotus_engine.h"

class ExperimentApplication : public Lotus::RenderingApplication
{
public:
  ExperimentApplication(const std::string& experimentName) : Lotus::RenderingApplication(experimentName, 720, 720)
  {
    LOTUS_ENABLE_PROFILING();

    experimentConfigured = false;



    /*
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
    }*/
  }

  virtual void update(float deltaTime) override
  {
    if (!experimentConfigured)
    {
      return;
    }

    Lotus::RenderingApplication::update(deltaTime);
  }
  
  virtual void render() override
  {
    if (!experimentConfigured)
    {
      return;
    }

    Lotus::RenderingApplication::render();
  }

  virtual void renderGUI() override
  {
    if (!experimentConfigured)
    {      
      
      ImGui::Begin("Configuration");

      static int objectRenderingMethodNumber;

      ImGui::Text("Object Rendering Method:"); ImGui::SameLine();
      ImGui::RadioButton("Traditional", &objectRenderingMethodNumber, 0); ImGui::SameLine();
      ImGui::RadioButton("Indirect", &objectRenderingMethodNumber, 1);

      objectRenderingMethod = objectRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional; 

      renderConfigurationGUI();

      if (ImGui::Button("Launch"))
      {
        experimentConfigured = true;
        initializeExperiment();
      }

      ImGui::End();
    }
    else
    {
      renderPostConfigurationGUI();
    }
  }

  virtual void renderConfigurationGUI() {}
  virtual void renderPostConfigurationGUI() {}
  virtual void initializeExperiment() {}

protected:

  bool experimentConfigured;

  Lotus::RenderingMethod objectRenderingMethod;
};