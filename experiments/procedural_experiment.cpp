#include "experiment_application.h"

Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

class TerrainExperimentApplication : public ExperimentApplication
{
public:

  TerrainExperimentApplication() : ExperimentApplication("Procedural"), objectsSpacing(10.0f)
  {}
  
private:

  virtual void setExperimentContext() override
  {
    context.set("ObjectsSpacing", std::to_string(objectsSpacing));
  }

  virtual void initializeExperiment() override
  {
    Lotus::PerlinNoiseConfig noiseConfiguration;
    dataGenerator = std::make_shared<Lotus::ProceduralDataGenerator>(512, 6, noiseConfiguration);

    setBackgroundColor(glm::vec3(0.5, 0.4, 0.4));

    renderingServer.setAmbientLight(glm::vec3(0.33, 0.33, 0.33));

    createDirectionalLight();
    createTerrain();
    createObjectPlacer();
  }

  virtual void updateExperiment(float deltaTime) override
  {
    glm::vec3 cameraPosition = camera.getLocalTranslation();
    dataGenerator->registerObserverPosition(glm::vec2(cameraPosition.x, cameraPosition.z));

    objectPlacer->update();
  }

  virtual void renderConfigurationGUI() override
  {
    ImGui::Text("Objects spacing:");
    ImGui::Dummy(ImVec2(0.0f, 4.0f));

    ImGui::PushItemWidth(configurationContentWindowWidth);
    ImGui::SliderFloat("##ObjectsSpacingSlider", &objectsSpacing, 1 << 0, 1 << 6);
    ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(0.0f, 12.0f));

  }

  virtual void renderPostConfigurationGUI() override
  {
    renderInformationWindow();
  }

  void renderInformationWindow()
  {
    ImGuiWindowFlags flags =  ImGuiWindowFlags_NoMove |
                              ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoSavedSettings |
                              ImGuiWindowFlags_NoBringToFrontOnFocus |
                              ImGuiWindowFlags_AlwaysAutoResize;

    ImVec2 windowPadding(12.0f, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);

    float windowWidth = ImGui::GetWindowWidth();

    if (ImGui::Begin("Information", nullptr, flags))
    {
      ImGui::SeparatorText("PDG");

      std::string chunkDimensionsText = "Chunk resolution: " + std::to_string(dataGenerator->getDataPerChunkSide()) + "x" + std::to_string(dataGenerator->getDataPerChunkSide());
      ImGui::Text(chunkDimensionsText.c_str());

      std::string gridDimensionsText = "Grid resolution: " + std::to_string(dataGenerator->getChunksPerSide()) + "x" + std::to_string(dataGenerator->getChunksPerSide());
      ImGui::Text(gridDimensionsText.c_str());

      ImGui::SeparatorText("OP");

      std::string chunksLoadedText = "Chunks loaded: " + std::to_string(objectPlacer->getChunksLoaded());
      ImGui::Text(chunksLoadedText.c_str());
    }

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void createDirectionalLight()
  {
    std::shared_ptr<Lotus::DirectionalLight> directionalLight = renderingServer.createDirectionalLight();

    directionalLight->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
    directionalLight->rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f));
    directionalLight->setLightColor(glm::vec3(0.7f, 0.4f, 0.15f));
  }

  void createTerrain()
  {
    std::shared_ptr<Lotus::Terrain> terrain = renderingServer.createTerrain(dataGenerator);
    terrain->setColor({ 0.25, 0.5, 0.0 });
  }

  void createObjectPlacer()
  {
    std::shared_ptr<Lotus::Mesh> rockAMesh = meshManager.loadMesh(Lotus::assetPath("models/nature/obj/rock_a.obj"));
    std::shared_ptr<Lotus::Mesh> rockBMesh = meshManager.loadMesh(Lotus::assetPath("models/nature/obj/rock_b.obj"));
    std::shared_ptr<Lotus::Mesh> treeAMesh = meshManager.loadMesh(Lotus::assetPath("models/nature/obj/tree_a_green.obj"));
    std::shared_ptr<Lotus::Mesh> treeBMesh = meshManager.loadMesh(Lotus::assetPath("models/nature/obj/tree_b_green.obj"));
    
    std::shared_ptr<Lotus::DiffuseFlatMaterial> rockMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    std::shared_ptr<Lotus::DiffuseFlatMaterial> lightTreeMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));
    std::shared_ptr<Lotus::DiffuseFlatMaterial> darkTreeMaterial = std::static_pointer_cast<Lotus::DiffuseFlatMaterial>(renderingServer.createMaterial(Lotus::MaterialType::DiffuseFlat));

    rockMaterial->setDiffuseColor({ 0.5, 0.5, 0.5 });
    lightTreeMaterial->setDiffuseColor({ 0.04, 0.3, 0.1 });
    darkTreeMaterial->setDiffuseColor({ 0.1, 0.3, 0.0 });

    objectPlacer = std::make_shared<Lotus::ObjectPlacer>(dataGenerator, &renderingServer, Lotus::RenderingMethod::Indirect, objectsSpacing);

    objectPlacer->addObject(rockAMesh, rockMaterial, true);
    objectPlacer->addObject(rockBMesh, rockMaterial, true);
    objectPlacer->addObject(treeBMesh, lightTreeMaterial);
    objectPlacer->addObject(treeBMesh, darkTreeMaterial);
    
    objectPlacer->initialize();
  }


  float objectsSpacing; 

  std::shared_ptr<Lotus::ProceduralDataGenerator> dataGenerator;
  std::shared_ptr<Lotus::ObjectPlacer> objectPlacer;

};

int main()
{
  TerrainExperimentApplication application;

  application.run();

  return 0;
}