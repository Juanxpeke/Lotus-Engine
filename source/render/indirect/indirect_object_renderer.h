#pragma once

#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include "../../math/types.h"
#include "../../math/render_primitives.h"
#include "../../math/gpu_primitives.h"
#include "../../scene/transform.h"
#include "../../scene/camera.h"
#include "../mesh.h"
#include "../gpu_buffer.h"
#include "../shader.h"
#include "../material.h"
#include "../mesh_object.h"


namespace Lotus
{
  class IndirectObjectRenderer
  {
  public:
    static constexpr unsigned int VertexBufferInitialAllocationSize = 1 << 16; 
    static constexpr unsigned int IndexBufferInitialAllocationSize = 1 << 16;
    static constexpr unsigned int IndirectBufferInitialAllocationSize = 1 << 10;
    static constexpr unsigned int ObjectBufferInitialAllocationSize = 1 << 10;
    static constexpr unsigned int MaterialBufferInitialAllocationSize = 1 << 8;

    IndirectObjectRenderer();
    ~IndirectObjectRenderer();

    std::shared_ptr<MeshObject> createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

    void render();

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
    void refreshObjectBuffer();
    void refreshObjectHandleBuffer();
    void refreshMaterialBuffer();

  private:


    // Util Functions
    Handle<RenderMesh> getMeshHandle(std::shared_ptr<Mesh> mesh);
    Handle<RenderMaterial> getMaterialHandle(std::shared_ptr<Material> material);

    // Shaders
    std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount)> shaders;

    // Maps
	  std::unordered_map<std::shared_ptr<Mesh>, Handle<RenderMesh>> meshMap;
	  std::unordered_map<std::shared_ptr<Material>, Handle<RenderMaterial>> materialMap;

    // Objects
    std::vector<std::shared_ptr<MeshObject>> meshInstances;
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

    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;

    DrawIndirectBuffer indirectBuffer;

    ShaderStorageBuffer<GPUObjectData> objectBuffer;
    ShaderStorageBuffer<uint32_t> objectHandleBuffer;
    ShaderStorageBuffer<GPUMaterialData> materialBuffer;

    // Extensions support
    bool supportsTexturedMaterials;
  };
}