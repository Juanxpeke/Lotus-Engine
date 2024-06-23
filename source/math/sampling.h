#pragma once

#include <cmath>
#include <algorithm>
#include <vector>
#include "linear_algebra.h"
#include "rng.h"

namespace Lotus
{
  class PoissonDiscSampler
  {
  public:

    static std::vector<Vec2f> samplePoints(float radius, float sampleRegionWidth, float sampleRegionHeight, uint8_t samplesBeforeRejection)
    {
      RNG rng;

      float cellSize = radius / std::sqrt(2);
      int cellsWidth = std::ceil(sampleRegionWidth / cellSize);
      int cellsHeight = std::ceil(sampleRegionHeight / cellSize);

      std::vector<int> grid;
      grid.resize(cellsWidth * cellsHeight);
      std::fill(grid.begin(), grid.end(), -1);

      std::vector<Vec2f> points;
      std::vector<Vec2f> spawnPoints;

      Vec2f initialPoint(rng.getFloatRange(sampleRegionWidth), rng.getFloatRange(sampleRegionHeight));
      addPointToGrid(points.size(), initialPoint, cellSize, cellsWidth, grid);
      points.push_back(initialPoint);
      spawnPoints.push_back(initialPoint);

      while (!spawnPoints.empty())
      {
        int spawnIndex = rng.getIntRange(0, spawnPoints.size() - 1);
        const Vec2f& spawnCentre = spawnPoints[spawnIndex];

        bool foundValidPoint = false;

        for (uint8_t i = 0; i < samplesBeforeRejection; i++)
        {
          float angle = rng.getFloat() * 2 * 3.1415;

          Vec2f direction(std::sin(angle), std::cos(angle));
          Vec2f candidate = spawnCentre + direction * rng.getFloatRange(radius, 2 * radius);

          if (isValidPoint(candidate, radius, sampleRegionWidth, sampleRegionHeight, cellSize, cellsWidth, cellsHeight, points, grid))
          {
            addPointToGrid(points.size(), initialPoint, cellSize, cellsWidth, grid);
            points.push_back(candidate);
            spawnPoints.push_back(candidate);

            foundValidPoint = true;
            break;
          }
        }

        if (!foundValidPoint)
        {
          spawnPoints[spawnIndex] = spawnPoints.back();
          spawnPoints.pop_back();
        }
      }

      return points;
    }

  private:

    static void addPointToGrid(
        int pointIndex,
        const Vec2f&point,
        float cellSize,
        int cellsWidth,
        std::vector<int>& grid)
    {
      Vec2i cell(point.x / cellSize, point.y / cellSize);

      grid[cell.y * cellsWidth + cell.x] = pointIndex; 
    }

    static bool isValidPoint(
        const Vec2f& point,
        float radius,
        float sampleRegionWidth,
        float sampleRegionHeight,
        float cellSize,
        int cellsWidth,
        int cellsHeight,
        const std::vector<Vec2f>& points,
        const std::vector<int>& grid)
    {
      if (point.x < 0 || point.x >= sampleRegionWidth || point.y < 0 || point.y >= sampleRegionHeight)
      {
        return false;
      }

      Vec2i cell(point.x / cellSize, point.y / cellSize);
      Vec2i startCell(std::max(cell.x - 1, 0), std::max(cell.y - 1, 0));
      Vec2i endCell(std::min(cell.x + 1, cellsWidth - 1), std::max(cell.y + 1, cellsHeight - 1));

      for (int x = startCell.x; x <= endCell.x; x++)
      {
        for (int y = startCell.y; y <= endCell.y; y++)
        {
          int pointIndex = grid[y * cellsWidth + x];
          
          if (pointIndex != -1 && (point - points[pointIndex]).sqrLength() < radius * radius)
          {
            return false;
          }
        }
      }

      return true;
    }
 
  };
}