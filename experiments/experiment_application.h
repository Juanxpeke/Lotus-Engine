#include "lotus_engine.h"

class ExperimentApplication : public Lotus::RenderingApplication
{
public:

  ExperimentApplication(const std::string& experimentName) :
    Lotus::RenderingApplication("Lotus Experiment - " + experimentName, 720, 720)
  {
    experimentConfigured = false;

    framesInHistory = LOTUS_GET_PROFILER_FRAME_HISTORY_MAX_SIZE();
    exportHistoryAutomatically = true;
    
    std::string exportPath = Lotus::experimentPath("results/" + experimentName + ".csv").string();
    std::memcpy(exportPathBuffer, exportPath.c_str(), (exportPath.size() + 1) * sizeof(char));

    disableVSync();
  }

  virtual void update(float deltaTime) override final
  {
    if (!experimentConfigured)
    {
      return;
    }

    Lotus::RenderingApplication::update(deltaTime);
    updateExperiment(deltaTime);
  }
  
  virtual void render() override final
  {
    if (!experimentConfigured)
    {
      return;
    }

    Lotus::RenderingApplication::render();
  }

  virtual void renderGUI() override final
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

      if (ImGui::Begin("Configuration", nullptr, flags))
      {
        configurationContentWindowWidth = ImGui::GetContentRegionAvail().x;
        configurationContentWindowHeight = ImGui::GetContentRegionAvail().y;
        configurationWindowWidth = ImGui::GetWindowWidth();
        configurationWindowHeight = ImGui::GetWindowHeight();

        ImVec2 titleSize = ImGui::CalcTextSize("Configuration");
        ImGui::SetCursorPosX((configurationWindowWidth - titleSize.x) * 0.5f);
        ImGui::Text("Configuration");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        ImGui::SeparatorText("Profiling");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Text("Frames in history:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::PushItemWidth(configurationContentWindowWidth);
        ImGui::SliderInt("##FramesInHistorySlider", &framesInHistory, 1, 1000);
        ImGui::PopItemWidth();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Checkbox("Export history automatically", &exportHistoryAutomatically);
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Text("Export path:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::PushItemWidth(configurationContentWindowWidth);
        ImGui::InputText("##ExportPathInput", exportPathBuffer, 1024);
        ImGui::PopItemWidth();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));


        ImGui::SeparatorText("Rendering");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        static int objectRenderingMethodNumber;

        ImGui::Text("Object rendering method:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::RadioButton("Traditional##ObjectRenderingTraditional", &objectRenderingMethodNumber, 0); ImGui::SameLine();
        ImGui::RadioButton("Indirect##ObjectRenderingIndirect", &objectRenderingMethodNumber, 1);
        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        
        static int terrainRenderingMethodNumber;

        ImGui::Text("Terrain rendering method:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::RadioButton("Traditional##TerrainRenderingTraditional", &terrainRenderingMethodNumber, 0); ImGui::SameLine();
        ImGui::RadioButton("Indirect##TerrainRenderingIndirect", &terrainRenderingMethodNumber, 1);
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::SeparatorText("Experiment");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        renderConfigurationGUI();
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        float buttonHeight = ImGui::GetFrameHeight();
        float cursorPosY = configurationWindowHeight - windowPadding.y - buttonHeight;

        if (ImGui::GetScrollMaxY() <= 0.0f)
        {
          ImGui::SetCursorPosY(cursorPosY);
        }

        if (ImGui::Button("Launch", ImVec2(configurationContentWindowWidth, 0)))
        {
          experimentConfigured = true;
          
          renderingServer.setDefaultObjectRenderingMethod(objectRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional);
          renderingServer.setDefaultTerrainRenderingMethod(terrainRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional); 

          std::string exportPath(exportPathBuffer);

          LOTUS_ENABLE_PROFILING();
          LOTUS_SET_PROFILER_FRAME_HISTORY_MAX_SIZE(framesInHistory);
          LOTUS_SET_PROFILER_EXPORT_AUTOMATIC(exportHistoryAutomatically);
          LOTUS_SET_PROFILER_EXPORT_PATH(exportPathBuffer);

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

  virtual void initializeExperiment() {}
  virtual void updateExperiment(float deltaTime) {}
  virtual void renderConfigurationGUI() {}
  virtual void renderPostConfigurationGUI() {}

protected:

  float configurationContentWindowWidth;
  float configurationContentWindowHeight;
  float configurationWindowWidth;
  float configurationWindowHeight;

private:

  bool experimentConfigured;

  int framesInHistory;
  bool exportHistoryAutomatically;
  char exportPathBuffer[1024];

};