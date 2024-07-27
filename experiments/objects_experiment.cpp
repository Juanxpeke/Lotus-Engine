#include "lotus_engine.h"
#include "experiment_application.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class ObjectsExperimentApplication : public ExperimentApplication
{
public:

  ObjectsExperimentApplication() : 
    ExperimentApplication("Objects"),
    regionSize(100.0f),
    numberOfObjects(1024),
    numberOfChangingMeshObjects(0),
    numberOfChangingMaterialObjects(0),
    numberOfChangingMaterialTypeObjects(0)
  {}
  
private:

  virtual void initializeExperiment() override
  {
    sphereMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);
    cubeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Cube);

    whiteUnlitMaterial = std::static_pointer_cast<Lotus::UnlitFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::UnlitFlat));
    whiteUnlitMaterial->setUnlitColor(glm::vec3(0.3f, 0.3f, 0.3f));
    blackUnlitMaterial = std::static_pointer_cast<Lotus::UnlitFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::UnlitFlat));
    blackUnlitMaterial->setUnlitColor(glm::vec3(0.1f, 0.1f, 0.1f));
    whiteDiffuseMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    whiteDiffuseMaterial->setDiffuseColor(glm::vec3(1.0f, 1.0f, 1.0f));
    blackDiffuseMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    blackDiffuseMaterial->setDiffuseColor(glm::vec3(0.1f, 0.1f, 0.1f));

    renderingServer.setAmbientLight(glm::vec3(0.2, 0.2, 0.2));

    createDirectionalLight();
    createObjects();    
  }

  virtual void updateExperiment(float deltaTime) override
  {
    changeObjectsMeshes();
    changeObjectsMaterials();
    changeObjectsMaterialTypes();
  }

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
    ImGui::SliderInt("##NumberOfObjectsSlider", &numberOfObjects, 1 << 0, 1 << 15);
    ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    if (numberOfChangingMeshObjects > numberOfObjects)
    {
      numberOfChangingMeshObjects = numberOfObjects;
    }

    ImGui::Text("Number of objects with changing mesh:");
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::PushItemWidth(configurationContentWindowWidth);
    ImGui::SliderInt("##NumberOfChangingMeshObjectsSlider", &numberOfChangingMeshObjects, 0, numberOfObjects);
    ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    if (numberOfChangingMaterialObjects > numberOfObjects)
    {
      numberOfChangingMaterialObjects = numberOfObjects;
    }

    ImGui::Text("Number of objects with changing material:");
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::PushItemWidth(configurationContentWindowWidth);
    ImGui::SliderInt("##NumberOfChangingMaterialObjectsSlider", &numberOfChangingMaterialObjects, 0, numberOfObjects);
    ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    if (numberOfChangingMaterialTypeObjects > numberOfObjects)
    {
      numberOfChangingMaterialTypeObjects = numberOfObjects;
    }

    ImGui::Text("Number of objects with changing material type:");
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::PushItemWidth(configurationContentWindowWidth);
    ImGui::SliderInt("##NumberOfChangingMaterialTypeObjectsSlider", &numberOfChangingMaterialTypeObjects, 0, numberOfObjects);
    ImGui::PopItemWidth();
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
    for (int i = 0; i < numberOfObjects; i++)
    {
      std::shared_ptr<Lotus::DiffuseFlatMaterial> initialMaterial = randomizer.getBool() ? whiteDiffuseMaterial : blackDiffuseMaterial;

      std::shared_ptr<Lotus::MeshObject> object = renderingServer.createObject(sphereMesh, initialMaterial);

      float x, y, z;
      x = randomizer.getFloatRange(-regionSize, regionSize);
      y = randomizer.getFloatRange(-regionSize, regionSize);
      z = randomizer.getFloatRange(-regionSize, regionSize);

      object->translate(glm::vec3(x, y, z));

      objects.push_back(object);
    }
  }

  void changeObjectsMeshes()
  {
    std::set<int> indices = generateDifferentObjectIndices(numberOfChangingMeshObjects);

    for (int index : indices)
    {
      std::shared_ptr<Lotus::MeshObject>& object = objects[index];

      if (object->getMesh() == sphereMesh)
      {
        object->setMesh(cubeMesh);
      }
      else
      {
        object->setMesh(sphereMesh);
      }
    }
  }

  void changeObjectsMaterials()
  {
    std::set<int> indices = generateDifferentObjectIndices(numberOfChangingMaterialObjects);

    for (int index : indices)
    {
      std::shared_ptr<Lotus::MeshObject>& object = objects[index];

      if (object->getMaterial() == whiteUnlitMaterial)
      {
        object->setMaterial(blackUnlitMaterial);
      }
      else if (object->getMaterial() == blackUnlitMaterial)
      {
        object->setMaterial(whiteUnlitMaterial);
      }
      else if (object->getMaterial() == whiteDiffuseMaterial)
      {
        object->setMaterial(blackDiffuseMaterial);
      }
      else
      {
        object->setMaterial(whiteDiffuseMaterial);
      }
    }
  }

  void changeObjectsMaterialTypes()
  {
    std::set<int> indices = generateDifferentObjectIndices(numberOfChangingMaterialTypeObjects);

    for (int index : indices)
    {
      std::shared_ptr<Lotus::MeshObject>& object = objects[index];

      if (object->getMaterial() == whiteUnlitMaterial)
      {
        object->setMaterial(whiteDiffuseMaterial);
      }
      else if (object->getMaterial() == blackUnlitMaterial)
      {
        object->setMaterial(blackDiffuseMaterial);
      }
      else if (object->getMaterial() == whiteDiffuseMaterial)
      {
        object->setMaterial(whiteUnlitMaterial);
      }
      else
      {
        object->setMaterial(blackUnlitMaterial);
      }
    }
  }

  std::set<int> generateDifferentObjectIndices(int amount)
  {
    std::set<int> indices;

    if ((amount > 1 << 14 && amount > objects.size() * 0.7) || amount > objects.size() * 0.9)
    {
      for (int i = 0; i < amount; i++)
      {
        indices.insert(i);
      }
    }
    else
    {
      while (indices.size() < amount)
      {
        int randomIndex = randomizer.getIntRange(0, objects.size() - 1);
        indices.insert(randomIndex);
      }
    }

    return indices;
  }

  Lotus::Randomizer randomizer;

  float regionSize;

  int numberOfObjects;
  int numberOfChangingMeshObjects;
  int numberOfChangingMaterialObjects;
  int numberOfChangingMaterialTypeObjects;

  std::vector<std::shared_ptr<Lotus::MeshObject>> objects;

  std::shared_ptr<Lotus::Mesh> sphereMesh;
  std::shared_ptr<Lotus::Mesh> cubeMesh;

  std::shared_ptr<Lotus::UnlitFlatMaterial> whiteUnlitMaterial;
  std::shared_ptr<Lotus::UnlitFlatMaterial> blackUnlitMaterial;
  std::shared_ptr<Lotus::DiffuseFlatMaterial> whiteDiffuseMaterial;
  std::shared_ptr<Lotus::DiffuseFlatMaterial> blackDiffuseMaterial;

};

int main()
{
	ObjectsExperimentApplication application;

  application.run();

  return 0;
}