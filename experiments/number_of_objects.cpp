#include "lotus_engine.h"
#include "experiment_application.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class NumberOfObjectsApplication : public ExperimentApplication
{
public:
  NumberOfObjectsApplication() : ExperimentApplication("NumberOfObjects"), numberOfObjects(1)
  {
  }
  
private:

  virtual void renderConfigurationGUI() override
  {
    ImGui::Text("Number of objects:");
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    
    int predefinedNumberOfObjects[7] = { 512, 1024, 2048, 4096, 8192, 16384, 32768 }; 

    for (int i = 0; i < 7; i++)
    {
      ImGui::RadioButton(std::to_string(predefinedNumberOfObjects[i]).c_str(), &numberOfObjects, predefinedNumberOfObjects[i]);

      if (i < 6)
      {
        ImGui::SameLine();
      }
      else
      {
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
      }
    }

    ImGui::PushItemWidth(configurationContentWindowWidth);
    ImGui::SliderInt("##slider1", &numberOfObjects, 1 << 0, 1 << 15);
    ImGui::PopItemWidth();

  }

  virtual void initializeExperiment() override
  {
    renderingServer.setAmbientLight(glm::vec3(0.1, 0.1, 0.1));
    
    createDirectionalLight();
    createObjects();    
  }

  void createDirectionalLight()
  {
    std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderingServer.createDirectionalLight();

    directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
    directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f));
    directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
  }

  void createObjects()
  {
    std::shared_ptr<Lotus::Mesh> sphereMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);

    std::shared_ptr<Lotus::DiffuseFlatMaterial> sphereMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));

    for (int i = 0; i < numberOfObjects; i++)
    {
      std::shared_ptr<Lotus::MeshObject> sphereObject = renderingServer.createObject(sphereMesh, sphereMaterial, objectRenderingMethod);

      float x, y, z;
      x = randomizer.getFloatRange(-100, 100); y = randomizer.getFloatRange(-100, 100); z = randomizer.getFloatRange(-100, 100);

      sphereObject->translate(glm::vec3(x, y, z));
    }
  }

  int numberOfObjects;
  Lotus::Randomizer randomizer;
};

int main()
{
	NumberOfObjectsApplication application;

  application.run();

  return 0;
}