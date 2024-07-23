#include <iostream>
#include <cstdlib>

#include "lotus_engine.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class ProceduralSceneApplication : public Lotus::RenderingApplication
{
public:
  ProceduralSceneApplication() : Lotus::RenderingApplication("Procedural Scene", 720, 720)
  {
    cameraSpeed = 128.0f;

    Lotus::PerlinNoiseConfig noiseConfiguration;
    dataGenerator = std::make_shared<Lotus::ProceduralDataGenerator>(512, 6, noiseConfiguration);

    renderingServer.setAmbientLight(glm::vec3(0.1, 0.1, 0.1));

    createDirectionalLight();
    createPointLights();
    createTerrain();
    createObjectPlacer();
  }
  
private:
  virtual void update(float deltaTime) override
  {

    glm::vec3 cameraPosition = camera.getLocalTranslation();
    dataGenerator->registerObserverPosition(glm::vec2(cameraPosition.x, cameraPosition.z));

    objectPlacer->update();

    Lotus::RenderingApplication::update(deltaTime);
  }

  void createDirectionalLight()
  {
    std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderingServer.createDirectionalLight();

    directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
    directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f));
    directionalLight->setLightColor(glm::vec3(0.1f, 0.04f, 0.0f));
  }

  void createPointLights()
  {
    std::shared_ptr<Lotus::PointLight> pointLight = renderingServer.createPointLight();

    pointLight->translate(glm::vec3(0.0f, 75.0f, 0.0f));
    pointLight->setLightColor(glm::vec3(1.0f, 1.0f, 1.0f));
    pointLight->setLightIntensity(200.f);
    pointLight->setLightRadius(80.f);
  }

  void createTerrain()
  {
    renderingServer.createTerrain(dataGenerator);
  }

  void createObjectPlacer()
  {
    std::shared_ptr<Lotus::Mesh> cubeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Cube);
    std::shared_ptr<Lotus::Mesh> sphereMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Sphere);
    
    std::shared_ptr<Lotus::DiffuseFlatMaterial> redMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    std::shared_ptr<Lotus::DiffuseFlatMaterial> blueMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    
    redMaterial->setDiffuseColor({ 1.0, 0.0, 0.0 });
    blueMaterial->setDiffuseColor({ 0.0, 0.0, 1.0 });

    objectPlacer = std::make_shared<Lotus::ObjectPlacer>(dataGenerator, &renderingServer, Lotus::RenderingMethod::Indirect, 36.0);

    objectPlacer->addObject(cubeMesh, redMaterial, 30.0);
    objectPlacer->addObject(sphereMesh, blueMaterial, 30.0);
    
    objectPlacer->initialize();
  }

  std::shared_ptr<Lotus::ProceduralDataGenerator> dataGenerator;
  std::shared_ptr<Lotus::ObjectPlacer> objectPlacer;
};

int main()
{
	ProceduralSceneApplication application;

  application.run();

  return 0;
}