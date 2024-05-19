#include "primitives.h"

namespace Lotus
{
  struct GPUDirectionalLightData
  {
    glm::vec3 colorIntensity; // 12
    float padding04;          // 16
    glm::vec3 direction;      // 28
    float padding08;          // 32
  };

  struct GPUPointLightData
  {
    glm::vec3 colorIntensity; // 12
    float padding04;          // 16
    glm::vec3 position;       // 28
    float radius;             // 32
  };

  struct GPUSpotLightData
  {
    glm::vec3 colorIntensity; // 12
    float radius;             // 16
    glm::vec3 position;       // 28
    float cosPenumbraAngle;   // 32
    glm::vec3 direction;      // 44
    float cosUmbraAngle;      // 48
  };

  /*
    Abstract representation of an unsigned int ID related to an specific class
  */
  template <typename T>
  class Handler
  {
  public:
    uint32_t get() const
    {
      return handle;
    }

    void set(uint32_t newHandle)
    {
      handle = newHandle;
    }

  private:
    uint32_t handle;
  };
  /*
   Representation of a mesh in the GPU buffers
 */
  struct DrawMesh
  {
      uint32_t count;
      uint32_t firstIndex;
      uint32_t baseVertex;
  };
  struct RenderObject
  {
    Handler<DrawMesh> meshHandler;
    Handler<int> materialHandle;

    glm::mat4 model;
  };
  struct PassObject
  {
      // PassMaterial material;
      Handler<DrawMesh> meshHandler;
      Handler<RenderObject> objectHandler;
      int32_t builtbatch;
      uint32_t customKey;
  };
  struct RenderBatch
  {
    Handler<PassObject> objectHandler;
    uint64_t sortKey;
  };

  struct PassMaterial
  {

  };



  struct GPUInstance
  {
    uint32_t objectID;
    uint32_t drawBatchID;
  };

  /*
    Shader batch
  */
  struct ShaderBatch
  {
		uint32_t first;
		uint32_t count;
  };

  /*
    Batch for draw command
  */
  struct DrawBatch
  {
    Handler<DrawMesh> meshHandler;
    uint32_t prevInstancesCount;
    uint32_t instancesCount;
  };

 
  struct DrawElementsIndirectCommand
  {
    uint32_t count = 0;         // # of indices
    uint32_t instanceCount = 0; // # of instances
    uint32_t firstIndex = 0;    // Index of the first element in the EBO
    uint32_t baseVertex = 0;    // Base value added to indices used in the EBO to read from the VBO
    uint32_t baseInstance = 0;  // gl_BaseInstance shader value
    uint32_t padding0 = 0;      // Padding due to GLSL layout std140 16 bytes alignment rule
    uint32_t padding1 = 0;
    uint32_t padding2 = 0;
  };

  struct GPUObjectData
  {
    glm::mat4 model;              // 64
    uint64_t materialHandle = 0;  // 72
    uint64_t padding0 = 0;        // 80
  };

  struct GPUMaterialData
  {
    glm::vec3 vec3_0;   // 12
    int int_0;          // 16
    glm::vec3 vec3_1;   // 28
    int int_1;          // 32
    uint64_t uint64_0;  // 40
    uint64_t uint64_1;  // 48
    uint64_t uint64_2;  // 56
    uint64_t uint64_3;  // 64
  };
}