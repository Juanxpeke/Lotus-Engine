#include "lotus_engine.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class SimpleSceneApplication : public Lotus::RenderingApplication
{
public:
  SimpleSceneApplication() : Lotus::RenderingApplication("Simple Scene", 720, 720)
  {
    renderingServer.setAmbientLight(glm::vec3(0.1, 0.1, 0.1));
    
    createDirectionalLight();
    createPointLights();
    createPlane();
    createVent();    
  }
  
private:

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

    pointLight->translate(glm::vec3(0.0f, 5.0f, 0.0f));
    pointLight->setLightColor(glm::vec3(1.0f, 1.0f, 1.0f));
    pointLight->setLightIntensity(25.f);
    pointLight->setLightRadius(400.f);
  }

  void createPlane()
  {
    std::shared_ptr<Lotus::Mesh> planeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane);

    std::shared_ptr<Lotus::GPUTexture> planeDiffuseTexture = textureLoader.loadTexture(Lotus::assetPath("textures/wood.png"));

    std::shared_ptr<Lotus::DiffuseTexturedMaterial> planeMaterial = std::static_pointer_cast<Lotus::DiffuseTexturedMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseTextured));

    planeMaterial->setDiffuseTexture(planeDiffuseTexture);

    std::shared_ptr<Lotus::MeshObject> planeObject = renderingServer.createObject(planeMesh, planeMaterial, Lotus::RenderingMethod::Indirect);

    planeObject->rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f));
    planeObject->scale(20.f);
  }

  void createVent()
  {
    std::shared_ptr<Lotus::Mesh> ventMesh = meshManager.loadMesh(Lotus::assetPath("models/air_conditioner/AirConditioner.obj"), true);

    std::shared_ptr<Lotus::GPUTexture> ventDiffuseTexture = textureLoader.loadTexture(Lotus::assetPath("models/air_conditioner/Albedo.png"));

    std::shared_ptr<Lotus::DiffuseTexturedMaterial> ventMaterial = std::static_pointer_cast<Lotus::DiffuseTexturedMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseTextured));

    ventMaterial->setDiffuseTexture(ventDiffuseTexture);

    std::shared_ptr<Lotus::MeshObject> ventObject = renderingServer.createObject(ventMesh, ventMaterial, Lotus::RenderingMethod::Indirect);

    ventObject->translate(glm::vec3(0.0f, 10.5f, -18.0f));
    ventObject->scale(0.3f);
  }

};

int main()
{
	SimpleSceneApplication application;

  application.run();

  return 0;
}