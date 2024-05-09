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


Mesh::Mesh(const std::string& filePath, bool flipUVs)
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

  size_t numVertices = 0;
  size_t numFaces = 0;

  // First, we count the number of vertices and total faces, so we can reserve memory and avoid memory reallocation
  for (uint32_t i = 0; i < scene->mNumMeshes; i++)
  {
    numVertices += scene->mMeshes[i]->mNumVertices;
    numFaces += scene->mMeshes[i]->mNumFaces;
  }

  vertices.reserve(numVertices);
  indices.reserve(numFaces);

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
        indices.push_back(face.mIndices[0] + offset);
        indices.push_back(face.mIndices[1] + offset);
        indices.push_back(face.mIndices[2] + offset);
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
}

Mesh::Mesh(PrimitiveType type)
{
  switch (type)
  {
    case Mesh::PrimitiveType::Plane:
    {
      LotusMath::Plane plane;
      vertices = plane.vertices;
      indices = plane.indices;
      break;
    }
    case Mesh::PrimitiveType::Cube:
    {
      LotusMath::Cube cube;
      vertices = cube.vertices;
      indices = cube.indices;
      break;
    }
    case Mesh::PrimitiveType::Sphere:
    {
      LotusMath::Sphere sphere;
      vertices = sphere.vertices;
      indices = sphere.indices;
      break;
    }
    default:
    {
      LotusMath::Sphere sphere;
      vertices = sphere.vertices;
      indices = sphere.indices;
      break;
    }
  }
}

Mesh::~Mesh()
{
  clearData();
}

void Mesh::clearData() noexcept
{
}