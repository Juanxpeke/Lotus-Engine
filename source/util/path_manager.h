#pragma once

/*
===================================
Placeholder script, don't modify it
===================================
*/

#include <iostream>
#include <string>
#include <filesystem>

namespace Lotus
{
  static const std::string directoryPath = "C:/Users/juani/Desktop/Bachelor Thesis/GPU-Driven-Renderer/";

  static std::filesystem::path assetPath(const std::string& assetFilename)
  {
    return directoryPath + "assets/" + assetFilename;
  }

  static std::filesystem::path shaderPath(const std::string& shaderFilename)
  {
    return directoryPath + "source/shaders/" + shaderFilename;
  }
}


