#include "mesh.h"

#include <iostream>
#include <vector>
#include <stack>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../util/log.h"
#include "../util/assimp_transformations.h"

namespace Lotus
{

  Mesh::Mesh(const std::string& filePath, bool flipUVs)
  {
    Assimp::Importer importer;
    unsigned int postProcessFlags = flipUVs ? aiProcess_FlipUVs : 0;
    postProcessFlags |= aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;
    const aiScene* scene = importer.ReadFile(filePath, postProcessFlags);

    if (!scene)
    {
      LOTUS_LOG_ERROR("[Mesh Error] Error trying to load mesh from path {0}", filePath);
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

          MeshVertex vertex;

          vertex.position = assimpToVec3(position);
          vertex.normal = assimpToVec3(normal);
          vertex.tangent = assimpToVec3(tangent);
          vertex.bitangent = assimpToVec3(bitangent);
          
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

          LOTUS_ASSERT(face.mNumIndices == 3, "[Mesh Error] Can't load meshes with face that have {0} vertices", face.mNumIndices);

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
        Plane plane;
        vertices = plane.vertices;
        indices = plane.indices;
        break;
      }
      case Mesh::PrimitiveType::Cube:
      {
        Cube cube;
        vertices = cube.vertices;
        indices = cube.indices;
        break;
      }
      case Mesh::PrimitiveType::Sphere:
      {
        Sphere sphere;
        vertices = sphere.vertices;
        indices = sphere.indices;
        break;
      }
      default:
      {
        Sphere sphere;
        vertices = sphere.vertices;
        indices = sphere.indices;
        break;
      }
    }
  }

  Mesh::~Mesh()
  {
  }


  Plane::Plane()
  {
    vertices = 
    {
      { { -1.0f, -1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
      { {  1.0f, -1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
      { {  1.0f,  1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
      { { -1.0f,  1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } }
    };

    indices =
    {
      0, 1, 2, 2, 3, 0
    };
  }

  Cube::Cube()
  {
    vertices =
    {
      { { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
      { {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
      { {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },
      { { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f } },

      { { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },
      { {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },
      { {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },
      { { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  1.0f,  0.0f } },

      { { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },
      { { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },
      { { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },
      { { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f } },

      { {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },
      { {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },
      { {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },
      { {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f } },

      { { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
      { {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
      { {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },
      { { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f,  1.0f } },

      { { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
      { {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
      { {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } },
      { { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f }, { 0.0f,  0.0f, -1.0f } }
    };

    indices =
    {
        0,  1,  2,  2,  3,  0,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
      12, 13, 14, 14, 15, 12,
      16, 17, 18, 18, 19, 16,
      20, 21, 22, 22, 23, 20
    };
  }

  Sphere::Sphere()
  {
    // Source: http://www.songho.ca/opengl/gl_sphere.html
    unsigned int stackCount = 16;
    unsigned int sectorCount = 32;

    constexpr float PI = glm::pi<float>();

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    float radius = 1.0f;

    float x, y, z;
    for (unsigned int i = 0; i <= stackCount; i++)
    {
      stackAngle = PI / 2.0f - i * stackStep;
      float cosStackAngle = std::cos(stackAngle);
      float sinStackAngle = std::sin(stackAngle);
      z = sinStackAngle;

      for (unsigned int j = 0; j <= sectorCount; j++)
      {
        sectorAngle = sectorStep * j;
        float cosSectorAngle = std::cos(sectorAngle);
        float sinSectorAngle = std::sin(sectorAngle);
        x = cosStackAngle * cosSectorAngle;
        y = cosStackAngle * sinSectorAngle;

        MeshVertex vertex;

        vertex.position = { radius * x, radius * y, radius * z};
        vertex.normal = { x, y, z };
        vertex.uv = { (float) j / (float) sectorCount, (float) i / (float) stackCount };
        vertex.tangent = { -sinSectorAngle, cosSectorAngle, 0.0f };
        vertex.bitangent = { -sinStackAngle * cosSectorAngle, -sinStackAngle * sinSectorAngle, cosStackAngle };

        vertices.push_back(vertex);
      }
    }

    unsigned int k1, k2;
    for (unsigned int i = 0; i < stackCount; ++i)
    {
      k1 = i * (sectorCount + 1);
      k2 = k1 + sectorCount + 1;

      for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
      {
        if (i != 0)
        {
          indices.push_back(k1);
          indices.push_back(k2);
          indices.push_back(k1 + 1);
        }

        if (i != (stackCount - 1))
        {
          indices.push_back(k1 + 1);
          indices.push_back(k2);
          indices.push_back(k2 + 1);
        }
      }
    }
  }
}