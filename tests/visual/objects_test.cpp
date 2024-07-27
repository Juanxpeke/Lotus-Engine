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
    meshes.push_back(meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere));
    meshes.push_back(meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Cube));
    meshes.push_back(meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane));
    
    std::shared_ptr<Lotus::UnlitFlatMaterial> redUnlitMaterial = std::static_pointer_cast<Lotus::UnlitFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::UnlitFlat));
    std::shared_ptr<Lotus::UnlitFlatMaterial> blueUnlitMaterial = std::static_pointer_cast<Lotus::UnlitFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::UnlitFlat));
    std::shared_ptr<Lotus::UnlitFlatMaterial> greenUnlitMaterial = std::static_pointer_cast<Lotus::UnlitFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::UnlitFlat));
    std::shared_ptr<Lotus::DiffuseFlatMaterial> redDiffuseMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    std::shared_ptr<Lotus::DiffuseFlatMaterial> blueDiffuseMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    std::shared_ptr<Lotus::DiffuseFlatMaterial> greenDiffuseMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));

    redUnlitMaterial->setUnlitColor(glm::vec3(1.0f, 0.0f, 0.0f));
    blueUnlitMaterial->setUnlitColor(glm::vec3(0.0f, 1.0f, 0.0f));
    greenUnlitMaterial->setUnlitColor(glm::vec3(0.0f, 0.0f, 1.0f));
    redDiffuseMaterial->setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
    blueDiffuseMaterial->setDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));
    greenDiffuseMaterial->setDiffuseColor(glm::vec3(0.0f, 0.0f, 1.0f));

    materials.push_back(redUnlitMaterial);
    materials.push_back(blueUnlitMaterial);
    materials.push_back(greenUnlitMaterial);
    materials.push_back(redDiffuseMaterial);
    materials.push_back(blueDiffuseMaterial);
    materials.push_back(greenDiffuseMaterial);

    renderingServer.setAmbientLight(glm::vec3(0.2, 0.2, 0.2));

    createDirectionalLight();   
  }

  virtual void updateTest(float deltaTime) override
  {
    if (selectedObject != nullptr)
    {
      selectedObject->setTranslation(glm::vec3(selectedTranslation[0], selectedTranslation[1], selectedTranslation[2]));
      selectedObject->setScale(glm::vec3(selectedScale[0], selectedScale[1], selectedScale[2]));

      if (meshes[selectedObjectMeshIndex] != selectedObject->getMesh())
      {
        selectedObject->setMesh(meshes[selectedObjectMeshIndex]);
      }
      if (materials[selectedObjectMaterialIndex] != selectedObject->getMaterial())
      {
        selectedObject->setMaterial(materials[selectedObjectMaterialIndex]);
      }
    }
    else if (selectedMaterial != nullptr)
    {
      std::shared_ptr<Lotus::UnlitFlatMaterial> unlitFlatMaterial;
      std::shared_ptr<Lotus::DiffuseFlatMaterial> diffuseFlatMaterial;
      
      switch (selectedMaterial->getType())
      {
        case Lotus::MaterialType::UnlitFlat:
          unlitFlatMaterial = std::static_pointer_cast<Lotus::UnlitFlatMaterial>(selectedMaterial);
          unlitFlatMaterial->setUnlitColor(glm::vec3(selectedColor[0], selectedColor[1], selectedColor[2]));
        case Lotus::MaterialType::DiffuseFlat:
          diffuseFlatMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(selectedMaterial);
          diffuseFlatMaterial->setDiffuseColor(glm::vec3(selectedColor[0], selectedColor[1], selectedColor[2]));
        default:
          return;
      }
    }
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
    renderCreationWindow();
    renderSelectionWindow();
  }

  void renderCreationWindow()
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

    if (ImGui::Begin("CreationWindow", nullptr, flags))
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
            std::shared_ptr<Lotus::MeshObject> object = renderingServer.createObject(meshes[0], materials[0]);
            objects.push_back(object);
          }

          for (int i = 0; i < objects.size(); i++)
          {
            renderObjectFrame(objects[i], i);
          }

          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Materials"))
        {
          for (int i = 0; i < materials.size(); i++)
          {
            renderMaterialFrame(materials[i], i);
          }

          ImGui::EndTabItem();
        }

        ImGui::PopItemWidth();

        ImGui::EndTabBar();
      }

      ImGui::PopStyleVar();
      ImGui::PopStyleVar();
    }

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void renderObjectFrame(const std::shared_ptr<Lotus::MeshObject>& object, int objectIndex)
  {
    if (ImGui::Selectable(("Object " + std::to_string(objectIndex)).c_str()))
    {
      const glm::vec3& objectTranslation = object->getLocalTranslation();
      const glm::vec3& objectScale = object->getLocalScale();

      selectedObjectIndex = objectIndex;

      selectedTranslation[0] = objectTranslation.x;
      selectedTranslation[1] = objectTranslation.y;
      selectedTranslation[2] = objectTranslation.z;
      
      selectedScale[0] = objectScale.x;
      selectedScale[1] = objectScale.y;
      selectedScale[2] = objectScale.z;

      for (int i = 0; i < meshes.size(); i++)
      {
        if (meshes[i] == object->getMesh())
        {
          selectedObjectMeshIndex = i;
        }
      }

      for (int i = 0; i < materials.size(); i++)
      {
        if (materials[i] == object->getMaterial())
        {
          selectedObjectMaterialIndex = i;
        }
      }

      selectedObject = object;
      selectedMaterial = nullptr;
    }
  }

  void renderMaterialFrame(const std::shared_ptr<Lotus::Material>& material, int materialIndex)
  {
    if (ImGui::Selectable(("Material " + std::to_string(materialIndex)).c_str()))
    {
      std::shared_ptr<Lotus::UnlitFlatMaterial> unlitFlatMaterial;
      std::shared_ptr<Lotus::DiffuseFlatMaterial> diffuseFlatMaterial;
      glm::vec3 materialColor;

      switch (material->getType())
      {
        case Lotus::MaterialType::UnlitFlat:
          unlitFlatMaterial = std::static_pointer_cast<Lotus::UnlitFlatMaterial>(material);
          materialColor = unlitFlatMaterial->getUnlitColor();
          selectedColor[0] = materialColor.x;
          selectedColor[1] = materialColor.y;
          selectedColor[2] = materialColor.z;
          break;
        case Lotus::MaterialType::DiffuseFlat:
          diffuseFlatMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(material);
          materialColor = diffuseFlatMaterial->getDiffuseColor();
          selectedColor[0] = materialColor.x;
          selectedColor[1] = materialColor.y;
          selectedColor[2] = materialColor.z;
          break;
        default:
          return;
      }

      selectedMaterialIndex = materialIndex;
      selectedMaterial = material;
      selectedObject = nullptr;
    }
  }

  void renderSelectionWindow()
  {
    ImGuiWindowFlags flags =  ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoSavedSettings |
                              ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_AlwaysAutoResize;

    ImVec2 windowPadding(12.0f, 12.0f);
    
    if (selectedObject != nullptr)
    {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);

      float windowWidth = ImGui::GetWindowWidth();

      /*
        The triple # allows to change the title without changing the window instance 
      */
      if (ImGui::Begin(("Object " + std::to_string(selectedObjectIndex) + "###SelectionWindow").c_str(), nullptr, flags))
      {
        ImGui::SeparatorText("Transform");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::Text("Translation:"); ImGui::SameLine(); ImGui::SliderFloat3("##translation", selectedTranslation, -50.0f, 50.0f);
        ImGui::Text("Scale:"); ImGui::SameLine(); ImGui::SliderFloat3("##scale", selectedScale, 0.0f, 10.0f);
        ImGui::Dummy(ImVec2(0.0f, 6.0f));

        ImGui::SeparatorText("Mesh");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::SliderInt("##mesh", &selectedObjectMeshIndex, 0, meshes.size() - 1);
        ImGui::Dummy(ImVec2(0.0f, 6.0f));

        ImGui::SeparatorText("Material");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::SliderInt("##material", &selectedObjectMaterialIndex, 0, materials.size() - 1);
      }

      ImGui::End();
      ImGui::PopStyleVar();
    }
    else if (selectedMaterial != nullptr)
    {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);

      float windowWidth = ImGui::GetWindowWidth();

      /*
        The triple # allows to change the title without changing the window instance 
      */
      if (ImGui::Begin(("Material " + std::to_string(selectedMaterialIndex) + "###SelectionWindow").c_str(), nullptr, flags))
      {
        ImGui::SeparatorText("Color");
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        ImGui::ColorPicker3("##color", selectedColor);
      }

      ImGui::End();
      ImGui::PopStyleVar();
    }
  }

  void createDirectionalLight()
  {
    std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderingServer.createDirectionalLight();

    directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
    directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f));
    directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
  }

  std::vector<std::shared_ptr<Lotus::MeshObject>> objects;
  std::shared_ptr<Lotus::MeshObject> selectedObject;
  int selectedObjectIndex;
  float selectedTranslation[3];
  float selectedScale[3];
  int selectedObjectMeshIndex;
  int selectedObjectMaterialIndex;

  std::vector<std::shared_ptr<Lotus::Mesh>> meshes;

  std::vector<std::shared_ptr<Lotus::Material>> materials;
  std::shared_ptr<Lotus::Material> selectedMaterial;
  int selectedMaterialIndex;
  float selectedColor[3];

};

int main()
{
	ObjectsTestApplication application;

  application.run();

  return 0;
}