#pragma once

#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include "../../math/types.h"
#include "../../scene/transform.h"
#include "../../scene/camera.h"
#include "../gpu_structures.h"
#include "../gpu_buffer.h"
#include "../mesh.h"
#include "../shader.h"
#include "../material.h"
#include "../mesh_object.h"
#include "indirect_render_structures.h"


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

    void update();
    void updateObjects();
    void updateMaterials();

    void buildBatches();
    void buildObjectBatches();
    void buildDrawBatches();
    void buildShaderBatches();

    void refreshBuffers();
    void refreshIndirectBuffer();
    void refreshObjectBuffer();
    void refreshObjectHandleBuffer();
    void refreshMaterialBuffer();

  private:


    // Util Functions
    Handle<RenderMesh> getMeshHandle(const std::shared_ptr<Mesh>& mesh);
    Handle<RenderMaterial> getMaterialHandle(const std::shared_ptr<Material>& material);

    // Shaders
    std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount)> shaders;

    // Maps
	  std::unordered_map<std::shared_ptr<Mesh>, Handle<RenderMesh>> meshMap;
	  std::unordered_map<std::shared_ptr<Material>, Handle<RenderMaterial>> materialMap;

    // Objects
    std::vector<std::shared_ptr<MeshObject>> objects;
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
    bool objectBatchesModified;
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