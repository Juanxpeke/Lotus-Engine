#include "object_placer.h"

#include <numeric>
#include <vector>
#include "../util/log.h"
#include "../math/sampling.h"

namespace Lotus
{
  ObjectPlacer::ObjectPlacer(const std::shared_ptr<ProceduralDataGenerator>& placerHeightsGenerator, float placerRadius, uint8_t placerSamplesBeforeRejection, uint32_t seed) :
    radius(placerRadius),
    samplesBeforeRejection(placerSamplesBeforeRejection),
    randomizer(seed),
    heightsGenerator(placerHeightsGenerator)
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

    const float totalMeshesWeight = std::accumulate(meshesWeights.begin(), meshesWeights.end(), 0.0f);
    const float* heightData = heightsGenerator->getChunkData(x ,y);

    std::vector<Vec2f> points = PoissonDiscSampler::samplePoints(radius, heightsGenerator->getDataPerChunkSide(), heightsGenerator->getDataPerChunkSide(), samplesBeforeRejection);

    for (const Vec2f& point : points)
    {
      Vec2i dataPoint(point.x, point.y);

      Vec3f translation = { point.x, heightData[dataPoint.y * heightsGenerator->getDataPerChunkSide() + dataPoint.x] , point.y };


    }
  }

}