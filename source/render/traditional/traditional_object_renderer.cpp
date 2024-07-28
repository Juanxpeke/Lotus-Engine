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

  std::shared_ptr<MeshObject> TraditionalObjectRenderer::createObject(const std::shared_ptr<Mesh>&  mesh, const std::shared_ptr<Material>& material)
  {
    LOTUS_PROFILE_INCREASE_COUNTER(FrameCounter::AddedTraditionalObjects);

    std::shared_ptr<MeshObject> object = std::make_shared<MeshObject>(mesh, material);
    objects.push_back(object);

    Handler<TraditionalRenderMesh> meshHandler = getMeshHandler(mesh);

    TraditionalRenderObject renderObject;
    renderObject.model = object->getModelMatrix();
    renderObject.mesh = meshHandler;

    renderObjects.push_back(renderObject);

    return object;
  }

  void TraditionalObjectRenderer::render()
  {
    updateObjects();

    LOTUS_PROFILE_START_TIME(FrameTime::TraditionalSceneRenderTime);

    for (int i = 0; i < objects.size(); i++)
    {
      const std::shared_ptr<MeshObject>& meshObject = objects[i];
      const std::shared_ptr<Material>& material = meshObject->getMaterial();
      
      const TraditionalRenderObject& renderObject = renderObjects[i];
      const TraditionalRenderMesh& renderMesh = renderMeshes[renderObject.mesh.handle];

      glUseProgram(shaders[static_cast<unsigned int>(material->getType())].getProgramID());

      material->setUniforms(renderObject.model);
      
      glBindVertexArray(renderMesh.gpuMesh->getVertexArrayID());
      
      glDrawElements(GL_TRIANGLES, renderMesh.gpuMesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }
    
    LOTUS_PROFILE_END_TIME(FrameTime::TraditionalSceneRenderTime);
  }

  void TraditionalObjectRenderer::updateObjects()
  {
    for (int i = 0; i < objects.size(); i++)
    {
      const std::shared_ptr<MeshObject>& object = objects[i];
      Transform* transform = &(object->transform);

      TraditionalRenderObject& renderObject = renderObjects[i];

      if (transform->dirty)
      {
        renderObject.model = object->getModelMatrix();
        transform->dirty = false;
      }
      if (object->meshDirty)
      {
        renderMeshes[renderObject.mesh.handle].references--;
        renderObject.mesh = getMeshHandler(object->getMesh());
        renderMeshes[renderObject.mesh.handle].references++;

        object->meshDirty = false;
      }
    }
  }

  Handler<TraditionalRenderMesh> TraditionalObjectRenderer::getMeshHandler(const std::shared_ptr<Mesh>& mesh)
  {
    Handler<TraditionalRenderMesh> handler;

    auto it = meshMap.find(mesh);

    if (it == meshMap.end())
    {
      const std::vector<MeshVertex>& vertices = mesh->getVertices();
      const std::vector<unsigned int>& indices = mesh->getIndices();

      TraditionalRenderMesh renderMesh;
      renderMesh.references++;
      renderMesh.gpuMesh = new GPUMesh(*mesh);

      handler.handle = static_cast<uint32_t>(renderMeshes.size());
      renderMeshes.push_back(renderMesh);

      meshMap[mesh] = handler;
    }
    else
    {
      handler = (*it).second;
    }

    return handler;
  }

}