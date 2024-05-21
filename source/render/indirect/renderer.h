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
#include "../../scene/transform.h"
#include "../../scene/camera.h"
#include "../../lighting/directional_light.h"
#include "../../lighting/point_light.h"
#include "mesh.h"
#include "shader_program.h"
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
    static constexpr unsigned int IndirectBufferInitialAllocationSize = 1 << 11;
    static constexpr unsigned int ObjectBufferInitialAllocationSize = 1 << 11;
    static constexpr unsigned int MaterialBufferInitialAllocationSize = 1 << 11;

    Renderer();
    ~Renderer();

    void startUp();

    std::shared_ptr<MeshInstance> createMeshInstance(std::shared_ptr<Mesh> mesh);
    
    void setAmbientLight(glm::vec3 color);
    std::shared_ptr<DirectionalLight> createDirectionalLight();
    std::shared_ptr<PointLight> createPointLight();  

    void render(const Camera& camera);

    // Objects Functions
    void updateObjects();

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
    Handler<DrawMesh> getMeshHandler(std::shared_ptr<Mesh> mesh);

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
	  std::unordered_map<std::shared_ptr<Mesh>, Handler<DrawMesh>> meshMap;
    
    // Lighting
    uint32_t lightBufferID;
    glm::vec3 ambientLight;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;
    std::vector<std::shared_ptr<PointLight>> pointLights;

    // Objects
    std::vector<std::shared_ptr<MeshInstance>> meshInstances;
    std::vector<RenderObject> objects;
    std::vector<Handler<RenderObject>> dirtyObjectsHandlers;
    std::vector<Handler<RenderObject>> toUnbatchObjectsHandlers;
    std::vector<Handler<RenderObject>> unbatchedObjectsHandlers;
    
    // Meshes
    std::vector<DrawMesh> meshes;

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

    GPUObjectData* CPUObjectBuffer;
    size_t objectBufferSize;
    size_t objectBufferAllocatedSize;
    uint32_t objectBufferID;

    GPUMaterialData* CPUMaterialBuffer;
    size_t materialBufferSize;
    size_t materialBufferAllocatedSize;
    uint32_t materialBufferID;

    uint32_t* CPUObjectHandleBuffer;
    size_t objectHandleBufferSize;
    size_t objectHandleBufferAllocatedSize;
    uint32_t objectHandleBufferID;

    GPUInstance* CPU_GPUInstanceBuffer;
    size_t CPU_GPUInstanceBufferSize;

  };
}