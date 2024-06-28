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
    heightsGenerator(placerHeightsGenerator),
    scene(indirectScene)
  {}

  void ObjectPlacer::addObject(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, float weight)
  {
    meshesPool.push_back(mesh);
    materialsPool.push_back(material);
    objectsWeights.push_back(weight);
  }

  void ObjectPlacer::removeObject(int objectIndex)
  {
    meshesPool[objectIndex] = meshesPool.back();
    materialsPool[objectIndex] = materialsPool.back();
    objectsWeights[objectIndex] = objectsWeights.back();

    meshesPool.pop_back();
    materialsPool.pop_back();
    objectsWeights.pop_back();

    // TODO: Solve indexing problem
  }

  void ObjectPlacer::generateAllObjects()
  {
    for (int x = 0; x < heightsGenerator->getChunksPerSide(); x++)
    {
      for (int y = 0; y < heightsGenerator->getChunksPerSide(); y++)
      {
        generateObjects(x, y);
      }
    }
  }

  void ObjectPlacer::generateObjects(const Vec2i& chunk)
  {
    generateObjects(chunk.x, chunk.y);
  }

  void ObjectPlacer::generateObjects(int x, int y)
  {
    if (meshesPool.empty())
    {
      LOTUS_LOG_WARN("[Object Placer Warning] Tried to generate objects, but the mesh pool is empty");
      return;
    }

    Vec2i chunkOffset(x - heightsGenerator->getChunksPerSide() / 2, y - heightsGenerator->getChunksPerSide() / 2);
    Vec2i offset = heightsGenerator->getDataOrigin() + chunkOffset * heightsGenerator->getDataPerChunkSide();
    Vec3f worldOffset(offset.x, 0, offset.y);

    const float totalObjectsWeight = std::accumulate(objectsWeights.begin(), objectsWeights.end(), 0.0f);
    const float* heightData = heightsGenerator->getChunkData(x ,y);

    LOTUS_LOG_INFO("[Object Placer Info] Generating points");
    std::vector<Vec2f> points = PoissonDiscSampler::samplePoints(radius, heightsGenerator->getDataPerChunkSide(), heightsGenerator->getDataPerChunkSide(), samplesBeforeRejection);
    LOTUS_LOG_INFO("[Object Placer Info] Finished points generation");

    for (const Vec2f& point : points)
    {
      Vec2i dataPoint(point.x, point.y);
      Vec3f translation = { point.x, heightData[dataPoint.y * heightsGenerator->getDataPerChunkSide() + dataPoint.x] , point.y };
      translation.y *= 64;

      translation += worldOffset;

      int objectIndex = randomizer.getIntRange(meshesPool.size() - 1);

      const std::shared_ptr<Mesh>& mesh = meshesPool[objectIndex];
      const std::shared_ptr<Material>& material = materialsPool[objectIndex];

      std::shared_ptr<MeshInstance> object = scene->createObject(mesh, material);
      object->setTranslation(glm::vec3(translation.x, translation.y, translation.z));
    }
  }

}