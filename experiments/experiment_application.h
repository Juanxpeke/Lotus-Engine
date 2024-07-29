#pragma once

#include <map>
#include "lotus_engine.h"

class ExperimentContext
{
public:

  void set(const std::string& key, const std::string& value)
  {
    context[key] = value;
  }

  void save() const
  {
    std::string filePath = Lotus::experimentPath("contexts.txt").string();
    std::ofstream contextsFile;

    if (std::filesystem::exists(filePath))
    {
      contextsFile.open(filePath, std::ios_base::app);
    }
    else
    {
      contextsFile.open(filePath);
    }

    if (!contextsFile.is_open())
    {
      LOTUS_LOG_ERROR("[Experiment] Unable to export experiment context");
      return;
    }

    for (const auto& pair : context)
    {
      contextsFile << pair.first << ":" << pair.second << std::endl;
    }

    contextsFile << "@@@@@@@@" << std::endl;

    contextsFile.close();
  }

private:

  std::map<std::string, std::string> context;

};

class ExperimentApplication : public Lotus::RenderingApplication
{
public:

  ExperimentApplication(const std::string& experimentName) :
    Lotus::RenderingApplication("Lotus Experiment - " + experimentName, 720, 720)
  {
    experimentConfigured = false;

    framesInHistory = 1000;

    Lotus::Randomizer randomizer;
    std::string resultKey = randomizer.getString(12);
    
    std::string exportPath = Lotus::experimentPath("results/" + resultKey + ".csv").string();
    std::memcpy(exportPathBuffer, exportPath.c_str(), (exportPath.size() + 1) * sizeof(char));

    context.set("Experiment", experimentName);
    context.set("Vendor", vendor);
    context.set("Device", device);

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

        ImGui::SeparatorText("Details");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Text("Vendor:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text(vendor.c_str());
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Text("Device:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text(device.c_str());
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::SeparatorText("Profiling");
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::Text("Frames in history:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::PushItemWidth(configurationContentWindowWidth);
        ImGui::SliderInt("##FramesInHistorySlider", &framesInHistory, 1, 1000);
        ImGui::PopItemWidth();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        static int exportHistoryAutomaticallyNumber;

        ImGui::Text("Export history automatically:");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::RadioButton("Yes##DoExportHistoryAutomatically", &exportHistoryAutomaticallyNumber, 0); ImGui::SameLine();
        ImGui::RadioButton("No##DoNotExportHistoryAutomatically", &exportHistoryAutomaticallyNumber, 1);
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
          LOTUS_SET_PROFILER_EXPORT_AUTOMATIC(exportHistoryAutomaticallyNumber ? false : true);
          LOTUS_SET_PROFILER_EXPORT_PATH(exportPathBuffer);

          context.set("ResultPath", exportPath);
          context.set("ObjectRenderingMethod", objectRenderingMethodNumber ? "Indirect" : "Traditional");
          context.set("TerrainRenderingMethod", terrainRenderingMethodNumber ? "Indirect" : "Traditional");

          setExperimentContext();

          auto lambda = [&]()
          {
            context.save();
          };

          LOTUS_SET_PROFILER_EXPORT_CALLBACK(lambda);

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
  
  virtual void setExperimentContext() {}
  virtual void initializeExperiment() {}
  virtual void updateExperiment(float deltaTime) {}
  virtual void renderConfigurationGUI() {}
  virtual void renderPostConfigurationGUI() {}

protected:

  ExperimentContext context;

  float configurationContentWindowWidth;
  float configurationContentWindowHeight;
  float configurationWindowWidth;
  float configurationWindowHeight;

private:

  bool experimentConfigured;

  int framesInHistory;
  char exportPathBuffer[1024];

};