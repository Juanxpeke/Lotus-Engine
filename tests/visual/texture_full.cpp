#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/path_manager.h"
#include "render/gpu_mesh.h"
#include "terrain/terrain_chunk_generator.h"
#include "render/indirect/mesh_manager.h"
#include "render/texture_loader.h"
#include "render/shader.h"

#define LOAD_IMAGE 0

int width = 360;
int height = 360;
char title[256];

float posX = 0.0;
float posY = 0.0;
float movementX = 0.0;
float movementY = 0.0;

float speed = 0.008;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  sprintf(title, "Texture Loader");
	GLFWwindow* window = glfwCreateWindow(width * 2, height * 2, title, NULL, NULL);

	if (window == NULL)
  {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width * 2, height * 2);

  Lotus::MeshManager& meshManager = Lotus::MeshManager::getInstance();
  Lotus::TextureLoader& textureLoader = Lotus::TextureLoader::getInstance();

  std::shared_ptr<Lotus::Mesh> planeMesh = meshManager.loadMesh(Lotus::Mesh::PrimitiveType::Plane); 
  Lotus::GPUMesh quadMesh(planeMesh->getVertices(), planeMesh->getIndices());

  Lotus::TextureConfig textureConfig;
  textureConfig.format = Lotus::TextureFormat::RFloat;
  textureConfig.width = width;
  textureConfig.height = height;
  textureConfig.depth = Lotus::ProceduralDataGenerator::ChunksAmount;

  Lotus::ProceduralDataGenerator terrainChunkGenerator(width);

  Lotus::GPUArrayTexture textureArray(textureConfig);

  for (int x = 0; x < Lotus::ProceduralDataGenerator::ChunksPerSide; x++)
  {
    for (int y = 0; y < Lotus::ProceduralDataGenerator::ChunksPerSide; y++)
    {
      uint16_t layer = y * Lotus::ProceduralDataGenerator::ChunksPerSide + x;
      textureArray.setLayerData(layer, terrainChunkGenerator.getChunkData(x, y));
    }
  }
  
  Lotus::ShaderProgram program(Lotus::testPath("shaders/texture_chunk.vert"), Lotus::testPath("shaders/texture_chunk.frag"));

  glBindVertexArray(quadMesh.getVertexArrayID());

  glUseProgram(program.getProgramID());

  glUniform1i(3, 0);

  glUniform1i(6, terrainChunkGenerator.getLeft());

  glBindTextureUnit(0, textureArray.getID());

  // Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
      posY += speed;
      movementY += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
      posY -= speed;
      movementY -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
      posX -= speed;
      movementX -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
      posX += speed;
      movementX += speed;
    }

    if (movementX > 1.0)
    {
      terrainChunkGenerator.updateRight();
      
      for (int y = 0; y < Lotus::ProceduralDataGenerator::ChunksPerSide; y++)
      {
        uint16_t layer = y * Lotus::ProceduralDataGenerator::ChunksPerSide + terrainChunkGenerator.getRight();
        textureArray.setLayerData(layer, terrainChunkGenerator.getChunkData(terrainChunkGenerator.getRight(), y));
      }
      
      movementX = 0.0;

      glUniform1i(6, terrainChunkGenerator.getLeft());
      
      LOTUS_LOG_INFO("UPDATING RIGHT");
    }
    else if (movementX < -1.0)
    {
      terrainChunkGenerator.updateLeft();

      for (int y = 0; y < Lotus::ProceduralDataGenerator::ChunksPerSide; y++)
      {
        uint16_t layer = y * Lotus::ProceduralDataGenerator::ChunksPerSide + terrainChunkGenerator.getLeft();
        textureArray.setLayerData(layer, terrainChunkGenerator.getChunkData(terrainChunkGenerator.getLeft(), y));
      }

      movementX = 0.0;
      
      glUniform1i(6, terrainChunkGenerator.getLeft());

      LOTUS_LOG_INFO("UPDATING LEFT");
    }
    if (movementY < -1.0)
    {
      terrainChunkGenerator.updateUp();
      
      for (int x = 0; x < Lotus::ProceduralDataGenerator::ChunksPerSide; x++)
      {
        uint16_t layer = terrainChunkGenerator.getUp() * Lotus::ProceduralDataGenerator::ChunksPerSide + x;
        textureArray.setLayerData(layer, terrainChunkGenerator.getChunkData(x, terrainChunkGenerator.getUp()));
      }
      
      movementY = 0.0;

      glUniform1i(7, terrainChunkGenerator.getUp());
      
      LOTUS_LOG_INFO("UPDATING UP");
    }
    else if (movementY > 1.0)
    {
      terrainChunkGenerator.updateDown();
      
      for (int x = 0; x < Lotus::ProceduralDataGenerator::ChunksPerSide; x++)
      {
        uint16_t layer = terrainChunkGenerator.getDown() * Lotus::ProceduralDataGenerator::ChunksPerSide + x;
        textureArray.setLayerData(layer, terrainChunkGenerator.getChunkData(x, terrainChunkGenerator.getDown()));
      }
      
      movementY = 0.0;

      glUniform1i(7, terrainChunkGenerator.getUp());
      
      LOTUS_LOG_INFO("UPDATING DOWN");
    }

    glUniform1fv(4, 1, &posX);
    glUniform1fv(5, 1, &posY);

    glDrawElements(GL_TRIANGLES, quadMesh.getIndicesCount(), GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

  return 0;
}