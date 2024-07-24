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
      ImGuiWindowFlags flags =  ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse |
                                ImGuiWindowFlags_NoTitleBar |
                                ImGuiWindowFlags_NoSavedSettings |
                                ImGuiWindowFlags_AlwaysAutoResize;

      ImVec2 increasedPadding(20.0f, 20.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, increasedPadding);

      const ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->Pos);
      ImGui::SetNextWindowSize(viewport->Size);

      if (ImGui::Begin("Experiment Configuration", nullptr, flags))
      {
        const char* title = "Experiment Configuration";
        ImVec2 titleSize = ImGui::CalcTextSize(title);
        ImGui::SetCursorPosX((viewport->Size.x - titleSize.x) * 0.5f);
        ImGui::Text(title);
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        static int objectRenderingMethodNumber;

        ImGui::Text("Object Rendering Method:");
        ImGui::Spacing();
        ImGui::PushID(0);
        ImGui::RadioButton("Traditional", &objectRenderingMethodNumber, 0); ImGui::SameLine();
        ImGui::PopID();
        ImGui::PushID(0);
        ImGui::RadioButton("Indirect", &objectRenderingMethodNumber, 1);
        ImGui::PopID();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        objectRenderingMethod = objectRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional; 
        
        static int terrainRenderingMethodNumber;

        ImGui::Text("Terrain Rendering Method:");
        ImGui::Spacing();
        ImGui::PushID(2);
        ImGui::RadioButton("Traditional", &terrainRenderingMethodNumber, 0); ImGui::SameLine();
        ImGui::PopID();
        ImGui::PushID(3);
        ImGui::RadioButton("Indirect", &terrainRenderingMethodNumber, 1);
        ImGui::PopID();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        terrainRenderingMethod = terrainRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional; 

        renderConfigurationGUI();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        float buttonWidth = ImGui::CalcTextSize("Launch").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        ImGui::SetCursorPosX((viewport->Size.x - buttonWidth) * 0.5f);

        if (ImGui::Button("Launch"))
        {
          experimentConfigured = true;
          initializeExperiment();
        }
      }

      ImGui::End();
      ImGui::PopStyleVar();
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
  Lotus::RenderingMethod terrainRenderingMethod;
};