#pragma once

#define GPU_BUFFERS_ENABLE_CPU_MAP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../../math/render_primitives.h"
#include "../../math/gpu_primitives.h"
#include "../../math/gpu_buffers.h"
#include "../../scene/transform.h"
#include "../../scene/camera.h"
#include "../../lighting/directional_light.h"
#include "../../lighting/point_light.h"
#include "../../terrain/terrain.h"
#include "mesh.h"
#include "../shader.h"
#include "material.h"
#include "unlit_flat_material.h"
#include "diffuse_flat_material.h"
#include "mesh_instance.h"


namespace Lotus
{
  class Renderer
  {
  public:
    static constexpr unsigned int ViewMatrixLocation = 0;
    static constexpr unsigned int ProjectionMatrixLocation = 1;

    static constexpr unsigned int HalfMaxDirectionalLights = 1;
    static constexpr unsigned int HalfMaxPointLights = 1;
    static constexpr unsigned int HalfMaxSpotLights = 1;

    static constexpr unsigned int LightUBOBindingPoint = 0;
    static constexpr unsigned int ObjectBufferBindingPoint = 0;
    static constexpr unsigned int ObjectHandleBufferBindingPoint = 1;
    static constexpr unsigned int MaterialBufferBindingPoint = 2;

    static constexpr unsigned int VertexBufferInitialAllocationSize = 1 << 16; 
    static constexpr unsigned int IndexBufferInitialAllocationSize = 1 << 16;
    static constexpr unsigned int IndirectBufferInitialAllocationSize = 1 << 10;
    static constexpr unsigned int ObjectBufferInitialAllocationSize = 1 << 10;
    static constexpr unsigned int MaterialBufferInitialAllocationSize = 1 << 8;

    Renderer();
    ~Renderer();

    void startUp();

    std::shared_ptr<MeshInstance> createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    void deleteMeshInstance(std::shared_ptr<MeshInstance> meshInstance);

    std::shared_ptr<Material> createMaterial(MaterialType type);

    std::shared_ptr<Terrain> createTerrain();

    void setAmbientLight(glm::vec3 color);
    std::shared_ptr<DirectionalLight> createDirectionalLight();
    std::shared_ptr<PointLight> createPointLight();  

    void render(const Camera& camera);

    // Update Functions
    void update();
    void updateObjects();
    void updateMaterials();

    // Batches Functions
    void buildBatches();
    void buildObjectBatches();
    void buildDrawBatches();
    void buildShaderBatches();

    // Buffers Functions
    void refreshBuffers();
    void refreshIndirectBuffer();
    void refreshLightBuffer();
    void refreshObjectBuffer();
    void refreshObjectHandleBuffer();
    void refreshMaterialBuffer();

    void refreshInstancesBuffer(); // TODO

  private:


    // Util Functions
    Handle<RenderMesh> getMeshHandle(std::shared_ptr<Mesh> mesh);
    Handle<RenderMaterial> getMaterialHandle(std::shared_ptr<Material> material);

    struct GPULightsData
    {
      GPUDirectionalLightData directionalLights[2 * HalfMaxDirectionalLights];
      GPUPointLightData pointLights[2 * HalfMaxPointLights];
      glm::vec3 ambientLight;
      int directionalLightsCount;
      int pointLightsCount;
    };

    // Shaders
    std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount)> shaders;

    // Maps
	  std::unordered_map<std::shared_ptr<Mesh>, Handle<RenderMesh>> meshMap;
	  std::unordered_map<std::shared_ptr<Material>, Handle<RenderMaterial>> materialMap;
    
    // Lighting
    uint32_t lightBufferID;
    glm::vec3 ambientLight;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;
    std::vector<std::shared_ptr<PointLight>> pointLights;

    // Objects
    std::vector<std::shared_ptr<MeshInstance>> meshInstances;
    std::vector<RenderObject> renderObjects;
    std::vector<Handle<RenderObject>> dirtyObjectsHandles;
    std::vector<RenderObject> toUnbatchObjects;
    std::vector<Handle<RenderObject>> unbatchedObjectsHandles;
    
    // Materials
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<RenderMaterial> renderMaterials;
    std::vector<Handle<RenderMaterial>> dirtyMaterialsHandles;

    // Meshes
    std::vector<RenderMesh> renderMeshes;

    // Batches
    std::vector<ObjectBatch> objectBatches;
    std::vector<DrawBatch> drawBatches;
    std::vector<ShaderBatch> shaderBatches;

    // Buffers
    uint32_t vertexArrayID;

    VertexBuffer GPUVertexBuffer;
    IndexBuffer GPUIndexBuffer;

    DrawIndirectBuffer GPUIndirectBuffer;
    ShaderStorageBuffer<GPUObjectData> GPUObjectBuffer;
    ShaderStorageBuffer<uint32_t> GPUObjectHandleBuffer;
    ShaderStorageBuffer<GPUMaterialData> GPUMaterialBuffer;

    // TODO
    GPUInstance* CPU_GPUInstanceBuffer;
    size_t CPU_GPUInstanceBufferSize;

    // Terrains
    ShaderProgram terrainShader;

    std::shared_ptr<Terrain> terrain;
  };
}