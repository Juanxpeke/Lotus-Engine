#include "object_placer.h"

#include <numeric>
#include <vector>
#include "../util/log.h"
#include "../math/sampling.h"

namespace Lotus
{
  ObjectPlacer::ObjectPlacer(
      const std::shared_ptr<ProceduralDataGenerator>& placerHeightsGenerator,
      RenderingServer* indirectScene,
      float placerRadius,
      uint8_t placerSamplesBeforeRejection,
      uint32_t seed) :
    radius(placerRadius),
    samplesBeforeRejection(placerSamplesBeforeRejection),
    randomizer(seed),
    dataGenerator(placerHeightsGenerator),
    renderingServer(indirectScene)
  {
    initialized = false;
  }

  void ObjectPlacer::initialize()
  {
    update(true);
    initialized = true;
  }

  void ObjectPlacer::addObject(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, float weight)
  {
    if (initialized)
    {
      LOTUS_LOG_WARN("[Object Placer Warning] Object added to placer pool, but placer is already initialized");
    }

    ObjectPlacerItem objectItem;
    objectItem.mesh = mesh;
    objectItem.material = material;

    objectItemsPool.push_back(objectItem);
  }

  void ObjectPlacer::update(bool forced)
  {
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::Everything) || forced)
    {
      for (int x = 0; x < dataGenerator->getChunksPerSide(); x++)
      {
        for (int y = 0; y < dataGenerator->getChunksPerSide(); y++)
        {
          generateObjects(x, y);
        }
      }
    }

    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::TopChunks))
    {
      for (int x = 0; x < dataGenerator->getChunksPerSide(); x++)
      {
        generateObjects(x, dataGenerator->getChunksTop());
      }
    }
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::RightChunks))
    {
      for (int y = 0; y < dataGenerator->getChunksPerSide(); y++)
      {
        generateObjects(dataGenerator->getChunksRight(), y);
      }
    }
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::BottomChunks))
    {
      for (int x = 0; x < dataGenerator->getChunksPerSide(); x++)
      {
        generateObjects(x, dataGenerator->getChunksBottom());
      }
    }
    if (dataGenerator->updatedSincePreviousFrame(ProceduralUpdateRegion::LeftChunks))
    {
      for (int y = 0; y < dataGenerator->getChunksPerSide(); y++)
      {
        generateObjects(dataGenerator->getChunksLeft(), y);
      }
    }
  }

  void ObjectPlacer::generateObjects(const glm::ivec2& chunk)
  {
    generateObjects(chunk.x, chunk.y);
  }

  void ObjectPlacer::generateObjects(int x, int y)
  {
    if (objectItemsPool.empty())
    {
      LOTUS_LOG_WARN("[Object Placer Warning] Tried to generate objects, but the object items pool is empty");
      return;
    }

    glm::uvec2 worldChunk;
    worldChunk.x = (x - dataGenerator->getChunksLeft() + dataGenerator->getChunksPerSide()) % dataGenerator->getChunksPerSide();
    worldChunk.y = (y - dataGenerator->getChunksTop() + dataGenerator->getChunksPerSide()) % dataGenerator->getChunksPerSide();
    glm::ivec2 chunkOffset(worldChunk.x - dataGenerator->getChunksPerSide() / 2, worldChunk.y - dataGenerator->getChunksPerSide() / 2);
    glm::ivec2 offset = dataGenerator->getDataOrigin() + chunkOffset * static_cast<int>(dataGenerator->getDataPerChunkSide());
    glm::vec3 worldOffset(offset.x, 0, offset.y);

    LOTUS_LOG_INFO("[Object Placer Info] Generating objects on chunk ({0}, {1})", offset.x / dataGenerator->getDataPerChunkSide(), offset.y / dataGenerator->getDataPerChunkSide());

    const float* heightData = dataGenerator->getChunkData(x ,y);

    std::vector<glm::vec2> points = PoissonDiscSampler::samplePoints(radius, dataGenerator->getDataPerChunkSide(), dataGenerator->getDataPerChunkSide(), samplesBeforeRejection);
  
    for (const glm::vec2& point : points)
    {
      glm::ivec2 dataPoint(point.x, point.y);
      glm::vec3 translation = { point.x, heightData[dataPoint.y * dataGenerator->getDataPerChunkSide() + dataPoint.x] , point.y };
      translation.y *= 64;

      translation += worldOffset;

      int objectIndex = randomizer.getIntRange(objectItemsPool.size() - 1);

      const ObjectPlacerItem& objectItem = objectItemsPool[objectIndex];

      std::shared_ptr<MeshInstance> object = renderingServer->createObject(objectItem.mesh, objectItem.material);
      object->setTranslation(translation);
    }
  }

}