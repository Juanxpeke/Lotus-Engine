#include "i_mesh.h"

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

struct MeshVertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 tangent;
  glm::vec3 bitangent;
};

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

  std::vector<MeshVertex> vertices;
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

  indexBufferCount = static_cast<uint32_t>(faces.size());

  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  glGenBuffers(1, &vertexBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, static_cast<unsigned int>(vertices.size()) * sizeof(MeshVertex), vertices.data(), GL_STATIC_DRAW);
  
  glGenBuffers(1, &indexBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>(faces.size()) * sizeof(unsigned int), faces.data(), GL_STATIC_DRAW);
  
  // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, uv));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tangent));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, bitangent));
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
      createPlane();
      break;
    }
    case Mesh::PrimitiveType::Cube:
    {
      createCube();
      break;
    }
    case Mesh::PrimitiveType::Sphere:
    {
      createSphere();
      break;
    }
    default:
    {
      createSphere();
      break;
    }
  }
}

Mesh::~Mesh()
{
  clearData();
}

void Mesh::createPrimitive(std::vector<float>& vertices, std::vector<unsigned int>& indices) noexcept
{
  unsigned int VAO, VBO, EBO;
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

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

void Mesh::createPlane() noexcept
{
  // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
  std::vector<float> planeVertices = 
  {
    -1.0f, -1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
  };

  std::vector<unsigned int> planeIndices =
  {
    0, 1, 2, 2, 3, 0
  };

  createPrimitive(planeVertices, planeIndices);
}

void Mesh::createCube() noexcept
{
  // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
  std::vector<float> cubeVertices =
  {
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,

    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f
  };

  std::vector<unsigned int> cubeIndices =
  {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
  };
  
  createPrimitive(cubeVertices, cubeIndices);
}

void Mesh::createSphere() noexcept
{
  // Source: http://www.songho.ca/opengl/gl_sphere.html
  // v = { p_x, p_y, p_z, n_x, n_y, n_z, uv_u, uv_v, t_x, t_y, t_z, b_x, b_y, b_z };
  std::vector<float> sphereVertices;
  std::vector<unsigned int> sphereIndices;

  unsigned int stackCount = 16;
  unsigned int sectorCount = 32;

  constexpr float PI = glm::pi<float>();
  
  float sectorStep = 2 * PI / sectorCount;
  float stackStep = PI / stackCount;
  float sectorAngle, stackAngle; // Phi and theta spherical cooordinates angles
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

      // Position
      sphereVertices.push_back(radius * x);
      sphereVertices.push_back(radius * y);
      sphereVertices.push_back(radius * z);

      // Normal
      sphereVertices.push_back(x);
      sphereVertices.push_back(y);
      sphereVertices.push_back(z);

      // UV
      float u = (float) j / (float) sectorCount;
      float v = (float) i / (float) stackCount;
      sphereVertices.push_back(u);
      sphereVertices.push_back(v);
      
      // Tangent = dr / dSectorAngle
      float tx = -sinSectorAngle;
      float ty = cosSectorAngle;
      float tz = 0.0f;
      sphereVertices.push_back(tx);
      sphereVertices.push_back(ty);
      sphereVertices.push_back(tz);
      
      // Bitangent = dr / dStackAngle
      float bx = -sinStackAngle * cosSectorAngle;
      float by = -sinStackAngle * sinSectorAngle;
      float bz = cosStackAngle;
      sphereVertices.push_back(bx);
      sphereVertices.push_back(by);
      sphereVertices.push_back(bz);
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
        sphereIndices.push_back(k1);
        sphereIndices.push_back(k2);
        sphereIndices.push_back(k1 + 1);
      }

      if (i != (stackCount - 1))
      {
        sphereIndices.push_back(k1 + 1);
        sphereIndices.push_back(k2);
        sphereIndices.push_back(k2 + 1);
      }
    }
  }

  createPrimitive(sphereVertices, sphereIndices);
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