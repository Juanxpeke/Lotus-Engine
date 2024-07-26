#include "lotus_engine.h"
#include "visual_test_application.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class ObjectsTestApplication : public VisualTestApplication
{
public:

  ObjectsTestApplication() : VisualTestApplication("Objects")
  {}
  
private:

  virtual void initializeTest() override
  {
    defaultMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);
    defaultMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));

    renderingServer.setAmbientLight(glm::vec3(0.2, 0.2, 0.2));

    createDirectionalLight();   
  }

  virtual void updateTest(float deltaTime) override
  {
  }

  virtual void renderConfigurationGUI() override
  {
    ImVec2 textSize = ImGui::CalcTextSize("No test configuration");
    ImGui::SetCursorPosX((configurationWindowWidth - textSize.x) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::Text("No test configuration");
    ImGui::PopStyleColor();
  }

  virtual void renderPostConfigurationGUI() override
  {
    ImGuiWindowFlags flags =  ImGuiWindowFlags_NoMove |
                              ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoTitleBar |
                              ImGuiWindowFlags_NoSavedSettings |
                              ImGuiWindowFlags_AlwaysAutoResize;

    ImVec2 windowPadding(0.0f, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x / 3, viewport->Size.y));

    float windowWidth = ImGui::GetWindowWidth();

    //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f)); // Remove horizontal spacing between tabs
    //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f); // Remove rounded corners for tabs

    if (ImGui::Begin("Objects", nullptr, flags))
    { 
      ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));


      if (ImGui::BeginTabBar("ObjectsTabs"))
      {
        ImGui::PushItemWidth(windowWidth / 2);
        
        if (ImGui::BeginTabItem("Objects"))
        {
          if (ImGui::Button("Add Object"))
          {
            std::shared_ptr<Lotus::MeshObject> object = renderingServer.createObject(defaultMesh, defaultMaterial);
            objects.push_back(object);
          }

          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Materials"))
        {
        // Add your content for the Materials tab here

          ImGui::EndTabItem();
        }

        ImGui::PopItemWidth();

        ImGui::EndTabBar();
      }

      ImGui::PopStyleVar(2);
    }

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void createDirectionalLight()
  {
    std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderingServer.createDirectionalLight();

    directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
    directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f));
    directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
  }

  std::vector<std::shared_ptr<Lotus::MeshObject>> objects;

  std::shared_ptr<Lotus::Mesh> defaultMesh;
  std::shared_ptr<Lotus::DiffuseFlatMaterial> defaultMaterial;

};

int main()
{
	ObjectsTestApplication application;

  application.run();

  return 0;
}