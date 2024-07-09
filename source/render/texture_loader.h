#pragma once

#include <memory>
#include <unordered_map>
#include <filesystem>
#include "../math/noise.h"
#include "gpu_texture.h"

namespace Lotus
{
  class TextureLoader
  {
  public:
    using TextureMap = std::unordered_map<std::string, std::shared_ptr<GPUTexture>>;

    TextureLoader(TextureLoader const&) = delete;

    TextureLoader& operator=(TextureLoader const&) = delete;

    static TextureLoader& getInstance() noexcept
    {
      static TextureLoader instance;
      return instance;
    }

    std::shared_ptr<GPUTexture> loadTexture(
        const std::filesystem::path& filePath,
        TextureMagnificationFilter magFilter = TextureMagnificationFilter::Linear,
        TextureMinificationFilter minFilter = TextureMinificationFilter::LinearMipmapLinear,
        TextureWrapMode sWrapMode = TextureWrapMode::Repeat,
        TextureWrapMode tWrapMode = TextureWrapMode::Repeat,
        bool genMipmaps = false) noexcept;

  private:
    TextureLoader() = default;

    TextureMap textureMap;
  };

}