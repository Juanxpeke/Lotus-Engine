#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/path_manager.h"
#include "render/gpu_mesh.h"
#include "render/indirect/mesh_manager.h"
#include "render/texture_loader.h"
#include "render/indirect/shader_program.h"

int width = 720;
int height = 720;
char title[256];

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "Perlin Noise Texture");
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL)
  {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

  Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
  Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();
  
  std::shared_ptr<Lotus::Mesh> planeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane); 
  Lotus::GPUMesh quadMesh(planeMesh->getVertices(), planeMesh->getIndices());
  Lotus::GPUTexture perlinNoiseTexture = textureLoader.generatePerlinTexture(width, height);

  //std::shared_ptr<Lotus::GPUTexture> ventDiffuseTexture = textureLoader.loadTexture(Lotus::assetPath("models/air_conditioner/Albedo.png"));

  Lotus::ShaderProgram program(Lotus::shaderPath("misc/texture.vert"), Lotus::shaderPath("misc/texture.frag"));

  glBindVertexArray(quadMesh.getVertexArrayID());

  glUseProgram(program.getProgramID());

  glUniform1i(0, 0);

  glBindTextureUnit(0, perlinNoiseTexture.getID());
  //glBindTextureUnit(0, ventDiffuseTexture->getID());

  // Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, quadMesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}