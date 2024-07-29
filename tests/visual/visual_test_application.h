#pragma once

#include "lotus_engine.h"

class VisualTestApplication : public Lotus::RenderingApplication
{
public:

  VisualTestApplication(const std::string& testName) :
    Lotus::RenderingApplication("Lotus Visual Test - " + testName, 720, 720)
  {
    testConfigured = false;
  }

  virtual void update(float deltaTime) override final
  {
    if (!testConfigured)
    {
      return;
    }

    Lotus::RenderingApplication::update(deltaTime);
    updateTest(deltaTime);
  }
  
  virtual void render() override final
  {
    if (!testConfigured)
    {
      return;
    }

    Lotus::RenderingApplication::render();
  }

  virtual void renderGUI() override final
  {
    if (!testConfigured)
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

        ImGui::SeparatorText("Test");
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
          testConfigured = true;
          
          renderingServer.setDefaultObjectRenderingMethod(objectRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional);
          renderingServer.setDefaultTerrainRenderingMethod(terrainRenderingMethodNumber ? Lotus::RenderingMethod::Indirect : Lotus::RenderingMethod::Traditional); 

          initializeTest();
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

  virtual void initializeTest() {}
  virtual void updateTest(float deltaTime) {}
  virtual void renderConfigurationGUI() {}
  virtual void renderPostConfigurationGUI() {}

protected:
  float configurationContentWindowWidth;
  float configurationContentWindowHeight;
  float configurationWindowWidth;
  float configurationWindowHeight;

private:
  bool testConfigured;

};