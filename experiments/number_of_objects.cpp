#include "lotus_engine.h"
#include "experiment_application.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class NumberOfObjectsApplication : public ExperimentApplication
{
public:
  NumberOfObjectsApplication() : ExperimentApplication("NumberOfObjects")
  {
    std::string objectMethodName = objectRenderingMethod == Lotus::RenderingMethod::Traditional ? "Traditional" : "Indirect";
    std::string profilerAppName = "NumberOfObjects-" + objectMethodName + "-" + std::to_string(numberOfObjects);

    LOTUS_SET_PROFILER_APP(profilerAppName);
  }
  
private:

  virtual void renderConfigurationGUI() override
  {
    ImGui::SliderInt("Number of objects", &numberOfObjects, 1 << 0, 1 << 15);
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