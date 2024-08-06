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
    std::shared_ptr<Lotus::Mesh> rockMesh = meshManager.loadMesh(Lotus::assetPath("models/nature/obj/rock_b.obj"));
    std::shared_ptr<Lotus::Mesh> treeMesh = meshManager.loadMesh(Lotus::assetPath("models/nature/obj/tree_b_green.obj"));
    
    std::shared_ptr<Lotus::DiffuseFlatMaterial> rockMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    std::shared_ptr<Lotus::DiffuseFlatMaterial> treeMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    
    rockMaterial->setDiffuseColor({ 0.5, 0.5, 0.5 });
    treeMaterial->setDiffuseColor({ 0.40, 0.35, 0.0 });

    float objectsSpacing = 12.0f; 

    objectPlacer = std::make_shared<Lotus::ObjectPlacer>(dataGenerator, &renderingServer, Lotus::RenderingMethod::Indirect, objectsSpacing);

    objectPlacer->addObject(rockMesh, rockMaterial, 30.0);
    objectPlacer->addObject(treeMesh, treeMaterial, 30.0);
    
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