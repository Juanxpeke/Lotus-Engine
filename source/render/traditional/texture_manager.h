#pragma once

#include <memory>
#include <unordered_map>
#include <filesystem>
#include "texture.h"

namespace Lotus
{
  namespace Traditional
  {
    class TextureManager
    {
    public:
      using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture>>;

      TextureManager(TextureManager const&) = delete;

      TextureManager& operator=(TextureManager const&) = delete;

      std::shared_ptr<Texture> loadTexture(
          const std::filesystem::path& filePath,
          TextureMagnificationFilter magFilter = TextureMagnificationFilter::Linear,
          TextureMinificationFilter minFilter = TextureMinificationFilter::LinearMipmapLinear,
          WrapMode sWrapMode = WrapMode::Repeat,
          WrapMode tWrapMode = WrapMode::Repeat,
          bool genMipmaps = false) noexcept;

      void cleanUnusedTextures() noexcept;
      
      static TextureManager& getInstance() noexcept
      {
        static TextureManager instance;
        return instance;
      }

    private:
      TextureManager() = default;

      void shutDown() noexcept;
      
      TextureMap textureMap;
    };
  }
}
