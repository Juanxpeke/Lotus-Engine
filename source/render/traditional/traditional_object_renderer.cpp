#include "traditional_object_renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../util/path_manager.h"

namespace Lotus
{

  TraditionalObjectRenderer::TraditionalObjectRenderer()
  {
    shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("traditional/diffuse_flat.vert"), shaderPath("traditional/diffuse_flat.frag"));
    shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(shaderPath("traditional/diffuse_textured.vert"), shaderPath("traditional/diffuse_textured.frag"));
  }

  void TraditionalObjectRenderer::render()
  {
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
  }

  std::shared_ptr<MeshObject> TraditionalObjectRenderer::createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
  {
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