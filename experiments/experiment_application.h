#include "lotus_engine.h"

class ExperimentApplication : public Lotus::RenderingApplication
{
public:
  ExperimentApplication(const std::string& experimentName) : Lotus::RenderingApplication(experimentName, 720, 720)
  {
    experimentConfigured = false;

    framesInHistory = LOTUS_GET_PROFILER_FRAME_HISTORY_MAX_SIZE();
    exportHistoryAutomatically = LOTUS_GET_PROFILER_EXPORT_AUTOMATIC();
    
    std::string exportPathString = LOTUS_GET_PROFILER_EXPORT_PATH();
    std::memcpy(exportPath, exportPathString.c_str(), (exportPathString.size() + 1) * sizeof(char));

    disableVSync();
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

      ImVec2 windowPadding(20.0f, 20.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);

      const ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->Pos);
      ImGui::SetNextWindowSize(viewport->Size);



      if (ImGui::Begin("Experiment Configuration", nullptr, flags))
      {
        configurationContentWindowWidth = ImGui::GetContentRegionAvail().x;
        configurationContentWindowHeight = ImGui::GetContentRegionAvail().y;
        configurationWindowWidth = ImGui::GetWindowWidth();
        configurationWindowHeight = ImGui::GetWindowHeight();

        ImVec2 titleSize = ImGui::CalcTextSize("Experiment Configuration");
        ImGui::SetCursorPosX((configurationWindowWidth - titleSize.x) * 0.5f);
        ImGui::Text("Experiment Configuration");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        ImGui::SeparatorText("Profiling");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Text("Frames in history:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::PushItemWidth(configurationContentWindowWidth);
        ImGui::SliderInt("##slider0", &framesInHistory, 1, 1000);
        ImGui::PopItemWidth();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Checkbox("Export history automatically", &exportHistoryAutomatically);
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Text("Export path:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::PushItemWidth(configurationContentWindowWidth);
        ImGui::InputText("##input0", exportPath, 1024);
        ImGui::PopItemWidth();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));


        ImGui::SeparatorText("Rendering");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        static int objectRenderingMethodNumber;

        ImGui::Text("Object rendering method:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::PushID(0);
        ImGui::RadioButton("Traditional", &objectRenderingMethodNumber, 0); ImGui::SameLine();
        ImGui::PopID();
        ImGui::PushID(1);
        ImGui::RadioButton("Indirect", &objectRenderingMethodNumber, 1);
        ImGui::PopID();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        
        static int terrainRenderingMethodNumber;

        ImGui::Text("Terrain rendering method:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::PushID(2);
        ImGui::RadioButton("Traditional", &terrainRenderingMethodNumber, 0); ImGui::SameLine();
        ImGui::PopID();
        ImGui::PushID(3);
        ImGui::RadioButton("Indirect", &terrainRenderingMethodNumber, 1);
        ImGui::PopID();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::SeparatorText("Experiment");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        renderConfigurationGUI();
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        float buttonHeight = ImGui::GetFrameHeight();
        float cursorPosY = configurationWindowHeight - windowPadding.y - buttonHeight;

        ImGui::SetCursorPosY(cursorPosY);
        ImGui::SetCursorPosX(windowPadding.x);

        if (ImGui::Button("Launch", ImVec2(configurationContentWindowWidth, 0)))
        {
          experimentConfigured = true;
          
          objectRenderingMethod = objectRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional;
          terrainRenderingMethod = terrainRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional; 

          std::string exportPathString(exportPath);

          LOTUS_ENABLE_PROFILING();
          LOTUS_SET_PROFILER_FRAME_HISTORY_MAX_SIZE(framesInHistory);
          LOTUS_SET_PROFILER_EXPORT_AUTOMATIC(exportHistoryAutomatically);
          LOTUS_SET_PROFILER_EXPORT_PATH(exportPathString);

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
  float configurationContentWindowWidth;
  float configurationContentWindowHeight;
  float configurationWindowWidth;
  float configurationWindowHeight;

  Lotus::RenderingMethod objectRenderingMethod;
  Lotus::RenderingMethod terrainRenderingMethod;

private:
  bool experimentConfigured;

  int framesInHistory;
  bool exportHistoryAutomatically;
  char exportPath[1024];
};