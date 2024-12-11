#include "lotus_engine.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class SimpleSceneApplication : public Lotus::RenderingApplication
{
public:
  SimpleSceneApplication() : Lotus::RenderingApplication("Simple Scene ", 720, 720)
  {
    renderingServer.setAmbientLight(glm::vec3(0.05, 0.05, 0.05));
    
    createPointLights();
    createPlane();
    createVent();
    createChairs();
  }
  
private:

  void createPointLights()
  {
    std::shared_ptr<Lotus::PointLight> pointLight = renderingServer.createPointLight();

    pointLight->translate(glm::vec3(0.0f, 22.0f, 0.0f));
    pointLight->setLightColor(glm::vec3(1.0f, 0.95f, 0.85f));
    pointLight->setLightIntensity(200.f);
    pointLight->setLightRadius(42.5f);
  }

  void createPlane()
  {
    std::shared_ptr<Lotus::Mesh> planeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane);

    std::shared_ptr<Lotus::GPUTexture> planeDiffuseTexture = textureLoader.loadTexture(Lotus::assetPath("textures/wood.png"));

    std::shared_ptr<Lotus::DiffuseTexturedMaterial> planeMaterial = std::static_pointer_cast<Lotus::DiffuseTexturedMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseTextured));

    planeMaterial->setDiffuseTexture(planeDiffuseTexture);

    std::shared_ptr<Lotus::MeshObject> planeObject = renderingServer.createObject(planeMesh, planeMaterial, Lotus::RenderingMethod::Traditional);

    planeObject->rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f));
    planeObject->scale(22.f);
  }

  void createVent()
  {
    std::shared_ptr<Lotus::Mesh> ventMesh = meshManager.loadMesh(Lotus::assetPath("models/air_conditioner/air_conditioner.obj"), true);

    std::shared_ptr<Lotus::GPUTexture> ventDiffuseTexture = textureLoader.loadTexture(Lotus::assetPath("models/air_conditioner/albedo.png"));

    std::shared_ptr<Lotus::DiffuseTexturedMaterial> ventMaterial = std::static_pointer_cast<Lotus::DiffuseTexturedMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseTextured));

    ventMaterial->setDiffuseTexture(ventDiffuseTexture);

    std::shared_ptr<Lotus::MeshObject> ventObject = renderingServer.createObject(ventMesh, ventMaterial, Lotus::RenderingMethod::Traditional);

    ventObject->translate(glm::vec3(0.0f, 8.5f, -18.5f));
    ventObject->scale(0.25f);
  }

  void createChairs()
  {
    std::shared_ptr<Lotus::Mesh> chairMesh = meshManager.loadMesh(Lotus::assetPath("models/chair/chair2.fbx"), true);

    std::shared_ptr<Lotus::DiffuseFlatMaterial> chairMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));

    chairMaterial->setDiffuseColor(glm::vec3(0.2f, 0.14f, 0.1f));

    std::shared_ptr<Lotus::MeshObject> chairObject1 = renderingServer.createObject(chairMesh, chairMaterial, Lotus::RenderingMethod::Traditional);
    std::shared_ptr<Lotus::MeshObject> chairObject2 = renderingServer.createObject(chairMesh, chairMaterial, Lotus::RenderingMethod::Traditional);

    chairObject1->translate(glm::vec3(-14.0f, 0.4f, 4.0f));
    chairObject1->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(110.0f));
    chairObject1->scale(0.25f);

    chairObject2->translate(glm::vec3(10.0f, 0.4f, 10.5f));
    chairObject2->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(210.0f));
    chairObject2->scale(0.25f);
  }

};

int main()
{
	SimpleSceneApplication application;

  application.run();

  return 0;
}