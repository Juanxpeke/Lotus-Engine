#pragma once

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
#include "mesh.h"
#include "shader_program.h"
#include "material.h"
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
    static constexpr unsigned int ObjectSSBOBindingPoint = 0;
    static constexpr unsigned int ObjectHandleSSBOBindingPoint = 1;
    static constexpr unsigned int MaterialSSBOBindingPoint = 2;

    static constexpr unsigned int VertexBufferInitialAllocationSize = 1 << 16; 
    static constexpr unsigned int IndexBufferInitialAllocationSize = 1 << 16;
    static constexpr unsigned int IndirectBufferInitialAllocationSize = 1 << 15;
    static constexpr unsigned int ObjectBufferInitialAllocationSize = 1 << 10;
    static constexpr unsigned int MaterialBufferInitialAllocationSize = 1 << 8;

    Renderer();
    ~Renderer();

    void startUp();

    std::shared_ptr<MeshInstance> createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    std::shared_ptr<Material> createMaterial(MaterialType type);

    void setAmbientLight(glm::vec3 color);
    std::shared_ptr<DirectionalLight> createDirectionalLight();
    std::shared_ptr<PointLight> createPointLight();  

    void render(const Camera& camera);

    // Update Functions
    void updateObjects();
    void updateMaterials();

    // Batches Functions
    void buildBatches();
    void buildRenderBatches();
    void buildDrawBatches();
    void buildShaderBatches();

    // Buffers Functions
    void refreshBuffers();
    void refreshIndirectBuffer();
    void refreshLightBuffer();
    void refreshObjectBuffer();
    void refreshObjectHandleBuffer();

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
    std::array<ShaderProgram, static_cast<unsigned int>(1)> shaders;

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
    std::vector<RenderObject> objects;
    std::vector<Handle<RenderObject>> dirtyObjectsHandles;
    std::vector<RenderObject> toUnbatchObjects;
    std::vector<Handle<RenderObject>> unbatchedObjectsHandlers;
    
    std::vector<std::shared_ptr<Material>> materials;

    // Meshes
    std::vector<RenderMesh> meshes;

    // Batches
    std::vector<RenderBatch> renderBatches;
    std::vector<DrawBatch> drawBatches;
    std::vector<ShaderBatch> shaderBatches;

    // Buffers
    uint32_t vertexArrayID;

    Vertex* CPUVertexBuffer;
    size_t vertexBufferSize;
    size_t vertexBufferAllocatedSize;
    uint32_t vertexBufferID;

    unsigned int* CPUIndexBuffer;
    size_t indexBufferSize;
    size_t indexBufferAllocatedSize;
    uint32_t indexBufferID;

    DrawElementsIndirectCommand* CPUIndirectBuffer;
    size_t indirectBufferSize;
    size_t indirectBufferAllocatedSize;
    uint32_t indirectBufferID;

    ShaderStorageBuffer<GPUObjectData> GPUObjectBuffer;
    ShaderStorageBuffer<GPUMaterialData> GPUMaterialBuffer;

    uint32_t* CPUObjectHandleBuffer;
    size_t objectHandleBufferSize;
    size_t objectHandleBufferAllocatedSize;
    uint32_t objectHandleBufferID;

    GPUInstance* CPU_GPUInstanceBuffer;
    size_t CPU_GPUInstanceBufferSize;

  };
}