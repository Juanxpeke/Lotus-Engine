#include "mesh.h"

#include <iostream>
#include <vector>
#include <stack>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../util/assimp_transformations.h"


Mesh::Mesh(const std::string& filePath, bool flipUVs) :
  vertexArrayID(0),
  vertexBufferID(0),
  indexBufferID(0),
  indexBufferCount(0)
{
  Assimp::Importer importer;
  unsigned int postProcessFlags = flipUVs ? aiProcess_FlipUVs : 0;
  postProcessFlags |= aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;
  const aiScene* scene = importer.ReadFile(filePath, postProcessFlags);

  if (!scene)
  {
    // TODO: Error log
    return;
  }

  std::vector<LotusMath::Vertex> vertices;
  std::vector<unsigned int> faces;
  size_t numVertices = 0;
  size_t numFaces = 0;

  // First, we count the number of vertices and total faces, so we can reserve memory and avoid memory reallocation
  for (uint32_t i = 0; i < scene->mNumMeshes; i++)
  {
    numVertices += scene->mMeshes[i]->mNumVertices;
    numFaces += scene->mMeshes[i]->mNumFaces;
  }

  vertices.reserve(numVertices);
  faces.reserve(numFaces);

  // Scene graph is traversed using Depth-first Search, with two stacks
  std::stack<const aiNode*> sceneNodes;
  std::stack<aiMatrix4x4> sceneTransforms;
  // Then we push information related to the graph root
  sceneNodes.push(scene->mRootNode);
  sceneTransforms.push(scene->mRootNode->mTransformation);
  unsigned int offset = 0;

  while (!sceneNodes.empty())
  {
    const aiNode* currentNode = sceneNodes.top();
    auto currentTransform = sceneTransforms.top();
    sceneNodes.pop();
    sceneTransforms.pop();
    auto currentInvTranspose = currentTransform;
    
    // Normals have to be transformed different than positions, thats why it is necessary to invert the matrix and then transpose it
    // Source: https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals
    currentInvTranspose.Inverse().Transpose();

    for (uint32_t j = 0; j < currentNode->mNumMeshes; j++)
    {
      const aiMesh* meshOBJ = scene->mMeshes[currentNode->mMeshes[j]];
      
      for (uint32_t i = 0; i < meshOBJ->mNumVertices; i++)
      {
        aiVector3D position = currentTransform * meshOBJ->mVertices[i];

        aiVector3D tangent = currentTransform * meshOBJ->mTangents[i];
        tangent.Normalize();

        aiVector3D normal = currentInvTranspose * meshOBJ->mNormals[i];
        normal.Normalize();
        
        aiVector3D bitangent = currentTransform * meshOBJ->mBitangents[i];
        bitangent.Normalize();

        LotusMath::Vertex vertex;

        vertex.position = assimpToGlmVec3(position);
        vertex.normal = assimpToGlmVec3(normal);
        vertex.tangent = assimpToGlmVec3(tangent);
        vertex.bitangent = assimpToGlmVec3(bitangent);
        
        if (meshOBJ->mTextureCoords[0])
        {
          vertex.uv.x = meshOBJ->mTextureCoords[0][i].x;
          vertex.uv.y = meshOBJ->mTextureCoords[0][i].y;
        }
        else
        {
          vertex.uv = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
      }

      for (uint32_t i = 0; i < meshOBJ->mNumFaces; i++)
      {
        const aiFace& face = meshOBJ->mFaces[i];
        // TODO: ASSERT(face.mNumIndices == 3, "Mesh Error: Can load meshes with faces that have {0} vertices", face.mNumIndices);
        faces.push_back(face.mIndices[0] + offset);
        faces.push_back(face.mIndices[1] + offset);
        faces.push_back(face.mIndices[2] + offset);
      }
      
      offset += meshOBJ->mNumVertices;
    }

    for (uint32_t j = 0; j < currentNode->mNumChildren; j++)
    {
      // Push the children and accumulate the transformation matrix
      sceneNodes.push(currentNode->mChildren[j]);
      sceneTransforms.push(currentNode->mChildren[j]->mTransformation * currentTransform);
    }
  }

  createMesh(vertices, faces);
}

Mesh::Mesh(PrimitiveType type) :
  vertexArrayID(0),
  vertexBufferID(0),
  indexBufferID(0),
  indexBufferCount(0)
{
  switch (type)
  {
    case Mesh::PrimitiveType::Plane:
    {
      LotusMath::Plane plane;
      createMesh(plane.vertices, plane.indices);
      break;
    }
    case Mesh::PrimitiveType::Cube:
    {
      LotusMath::Cube cube;
      createMesh(cube.vertices, cube.indices);
      break;
    }
    case Mesh::PrimitiveType::Sphere:
    {
      LotusMath::Sphere sphere;
      createMesh(sphere.vertices, sphere.indices);
      break;
    }
    default:
    {
      LotusMath::Sphere sphere;
      createMesh(sphere.vertices, sphere.indices);
      break;
    }
  }
}

Mesh::~Mesh()
{
  clearData();
}

void Mesh::createMesh(std::vector<LotusMath::Vertex>& vertices, std::vector<unsigned int>& indices) noexcept
{
  unsigned int VAO, VBO, EBO;
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LotusMath::Vertex), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, uv));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, tangent));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(LotusMath::Vertex), (void*) offsetof(LotusMath::Vertex, bitangent));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  vertexArrayID = VAO;
  vertexBufferID = VBO;
  indexBufferID = EBO;
  indexBufferCount = static_cast<uint32_t>(indices.size());
}

void Mesh::clearData() noexcept
{
  if (vertexArrayID)
  { // TODO: Handle errors
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    vertexArrayID = 0;
  }
}