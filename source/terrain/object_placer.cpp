#include "object_placer.h"

#include <numeric>
#include <vector>
#include "../util/log.h"
#include "../math/sampling.h"

namespace Lotus
{
  ObjectPlacer::ObjectPlacer(
      const std::shared_ptr<ProceduralDataGenerator>& placerHeightsGenerator,
      const std::shared_ptr<IndirectScene>& indirectScene,
      float placerRadius,
      uint8_t placerSamplesBeforeRejection,
      uint32_t seed) :
    radius(placerRadius),
    samplesBeforeRejection(placerSamplesBeforeRejection),
    randomizer(seed),
    dataGenerator(placerHeightsGenerator),
    scene(indirectScene)
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

  void ObjectPlacer::generateObjects(const Vec2i& chunk)
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

    Vec2u worldChunk;
    worldChunk.x = (x - dataGenerator->getChunksLeft() + dataGenerator->getChunksPerSide()) % dataGenerator->getChunksPerSide();
    worldChunk.y = (y - dataGenerator->getChunksTop() + dataGenerator->getChunksPerSide()) % dataGenerator->getChunksPerSide();
    Vec2i chunkOffset(worldChunk.x - dataGenerator->getChunksPerSide() / 2, worldChunk.y - dataGenerator->getChunksPerSide() / 2);
    Vec2i offset = dataGenerator->getDataOrigin() + chunkOffset * dataGenerator->getDataPerChunkSide();
    Vec3f worldOffset(offset.x, 0, offset.y);

    LOTUS_LOG_INFO("[Object Placer Info] Generating objects on chunk ({0}, {1})", offset.x / dataGenerator->getDataPerChunkSide(), offset.y / dataGenerator->getDataPerChunkSide());

    const float* heightData = dataGenerator->getChunkData(x ,y);

    std::vector<Vec2f> points = PoissonDiscSampler::samplePoints(radius, dataGenerator->getDataPerChunkSide(), dataGenerator->getDataPerChunkSide(), samplesBeforeRejection);
  
    for (const Vec2f& point : points)
    {
      Vec2i dataPoint(point.x, point.y);
      Vec3f translation = { point.x, heightData[dataPoint.y * dataGenerator->getDataPerChunkSide() + dataPoint.x] , point.y };
      translation.y *= 64;

      translation += worldOffset;

      int objectIndex = randomizer.getIntRange(objectItemsPool.size() - 1);

      const ObjectPlacerItem& objectItem = objectItemsPool[objectIndex];

      std::shared_ptr<MeshInstance> object = scene->createObject(objectItem.mesh, objectItem.material);
      object->setTranslation(glm::vec3(translation.x, translation.y, translation.z));
    }
  }

}