#pragma once

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include "randomizer.h"

namespace Lotus
{
  class PoissonDiscSampler
  {
  public:

    static std::vector<glm::vec2> samplePoints(float radius, float sampleRegionWidth, float sampleRegionHeight, uint8_t samplesBeforeRejection)
    {
      Randomizer randomizer;

      float cellSize = radius / std::sqrt(2);
      int cellsWidth = std::ceil(sampleRegionWidth / cellSize);
      int cellsHeight = std::ceil(sampleRegionHeight / cellSize);

      std::vector<int> grid;
      grid.resize(cellsWidth * cellsHeight);
      std::fill(grid.begin(), grid.end(), -1);

      std::vector<glm::vec2> points;
      std::vector<glm::vec2> spawnPoints;

      glm::vec2 initialPoint(randomizer.getFloatRange(sampleRegionWidth), randomizer.getFloatRange(sampleRegionHeight));

      addPointToGrid(points.size(), initialPoint, cellSize, cellsWidth, grid);
      points.push_back(initialPoint);
      spawnPoints.push_back(initialPoint);

      while (!spawnPoints.empty())
      {
        int spawnIndex = randomizer.getIntRange(0, spawnPoints.size() - 1);
        const glm::vec2& spawnCentre = spawnPoints[spawnIndex];

        bool foundValidPoint = false;

        for (uint8_t i = 0; i < samplesBeforeRejection; i++)
        {
          float angle = randomizer.getFloat() * 2 * 3.1415;

          glm::vec2 direction(std::sin(angle), std::cos(angle));
          glm::vec2 candidate = spawnCentre + direction * randomizer.getFloatRange(radius, 2 * radius);

          if (isValidPoint(candidate, radius, sampleRegionWidth, sampleRegionHeight, cellSize, cellsWidth, cellsHeight, points, grid))
          {
            addPointToGrid(points.size(), candidate, cellSize, cellsWidth, grid);
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
        const glm::vec2&point,
        float cellSize,
        int cellsWidth,
        std::vector<int>& grid)
    {
      glm::ivec2 cell(point.x / cellSize, point.y / cellSize);

      grid[cell.y * cellsWidth + cell.x] = pointIndex; 
    }

    static bool isValidPoint(
        const glm::vec2& point,
        float radius,
        float sampleRegionWidth,
        float sampleRegionHeight,
        float cellSize,
        int cellsWidth,
        int cellsHeight,
        const std::vector<glm::vec2>& points,
        const std::vector<int>& grid)
    {
      if (point.x < 0 || point.x >= sampleRegionWidth || point.y < 0 || point.y >= sampleRegionHeight)
      {
        return false;
      }

      glm::ivec2 cell(point.x / cellSize, point.y / cellSize);
      glm::ivec2 startCell(std::max(cell.x - 1, 0), std::max(cell.y - 1, 0));
      glm::ivec2 endCell(std::min(cell.x + 1, cellsWidth - 1), std::min(cell.y + 1, cellsHeight - 1));

      for (int x = startCell.x; x <= endCell.x; x++)
      {
        for (int y = startCell.y; y <= endCell.y; y++)
        {
          int pointIndex = grid[y * cellsWidth + x];
          
          if (pointIndex != -1 && glm::distance(point, points[pointIndex]) < radius)
          {
            return false;
          }
        }
      }

      return true;
    }
  };
}