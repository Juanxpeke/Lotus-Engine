#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "math/types.h"
#include "util/path_manager.h"
#include "render/gpu_mesh.h"
#include "render/mesh_manager.h"
#include "render/texture_loader.h"
#include "render/shader.h"

#define LOAD_IMAGE 0

int width = 360;
int height = 360;
char title[256];


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "Texture Loader");
	GLFWwindow* window = glfwCreateWindow(width * 4, height, title, NULL, NULL);

	if (window == NULL)
  {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width * 4, height);

  Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
  Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

  std::shared_ptr<Lotus::Mesh> planeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane); 
  Lotus::GPUMesh quadMesh(planeMesh->getVertices(), planeMesh->getIndices());

  Lotus::TextureConfig textureConfig;
  textureConfig.format = Lotus::TextureFormat::RFloat;
  textureConfig.width = width;
  textureConfig.height = height;
  textureConfig.depth = 4;

  Lotus::Perlin2DArray perlin2DArrayW0(width, height, glm::vec2(0 * width, 0));
  Lotus::Perlin2DArray perlin2DArrayW1(width, height, glm::vec2(1 * width, 0));
  Lotus::Perlin2DArray perlin2DArrayW2(width, height, glm::vec2(2 * width, 0));
  Lotus::Perlin2DArray perlin2DArrayW3(width, height, glm::vec2(3 * width, 0));

  Lotus::GPUArrayTexture textureArray(textureConfig);

  textureArray.setLayerData(0, perlin2DArrayW0.getData());
  textureArray.setLayerData(1, perlin2DArrayW1.getData());
  textureArray.setLayerData(2, perlin2DArrayW2.getData());
  textureArray.setLayerData(3, perlin2DArrayW3.getData());
  
  Lotus::ShaderProgram program(Lotus::testPath("shaders/texture_array.vert"), Lotus::testPath("shaders/texture_array.frag"));

  glBindVertexArray(quadMesh.getVertexArrayID());

  glUseProgram(program.getProgramID());

  glUniform1i(3, 0);

  glBindTextureUnit(0, textureArray.getID());

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