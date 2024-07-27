#include "traditional_object_renderer.h"

#include "../../util/opengl_entry.h"
#include "../../util/profile.h"
#include "../../util/path_manager.h"

namespace Lotus
{

  TraditionalObjectRenderer::TraditionalObjectRenderer()
  {
    shaders[static_cast<unsigned int>(MaterialType::UnlitFlat)] = ShaderProgram(shaderPath("traditional/unlit_flat.vert"), shaderPath("traditional/unlit_flat.frag"));
    shaders[static_cast<unsigned int>(MaterialType::UnlitTextured)] = ShaderProgram(shaderPath("traditional/unlit_textured.vert"), shaderPath("traditional/unlit_textured.frag"));
    shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("traditional/diffuse_flat.vert"), shaderPath("traditional/diffuse_flat.frag"));
    shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(shaderPath("traditional/diffuse_textured.vert"), shaderPath("traditional/diffuse_textured.frag"));
  }

  void TraditionalObjectRenderer::render()
  {
    LOTUS_PROFILE_START_TIME(FrameTime::TraditionalSceneRenderTime);

    for (int i = 0; i < meshObjects.size(); i++)
    {
      const std::shared_ptr<MeshObject>& meshObject = meshObjects[i];
      const std::shared_ptr<Material>& material = meshObject->getMaterial();
      const std::shared_ptr<GPUMesh>& mesh = meshObject->GPUMeshPtr;

      glUseProgram(shaders[static_cast<unsigned int>(material->getType())].getProgramID());

      material->setUniforms(meshObject->getModelMatrix());
      
      glBindVertexArray(mesh->getVertexArrayID());
      
      glDrawElements(GL_TRIANGLES, mesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }
    
    LOTUS_PROFILE_END_TIME(FrameTime::TraditionalSceneRenderTime);
  }

  std::shared_ptr<MeshObject> TraditionalObjectRenderer::createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
  {
    LOTUS_PROFILE_INCREASE_COUNTER(FrameCounter::AddedTraditionalObjects);

    std::shared_ptr<MeshObject> meshObject = std::make_shared<MeshObject>(mesh, material);
    std::shared_ptr<GPUMesh> GPUMeshSharedPtr;

    auto it = meshMap.find(mesh);
    
    if (it != meshMap.end())
    {
      GPUMeshSharedPtr = it->second;
    }
    else
    {
      GPUMesh* GPUMeshPtr = new GPUMesh(*mesh);
      GPUMeshSharedPtr = std::shared_ptr<GPUMesh>(GPUMeshPtr);

      meshMap.insert({ mesh, GPUMeshSharedPtr });
    }

    meshObject->GPUMeshPtr = GPUMeshSharedPtr;

    meshObjects.push_back(meshObject);

    return meshObject;
  }

}