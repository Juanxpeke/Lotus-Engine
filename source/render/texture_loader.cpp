#include "texture_loader.h"

#include <cstring>
#include <stb_image.h>
#include "../util/log.h"

namespace Lotus
{

  GPUTexture* loadImageTexture(
      const std::string& filePath,
      TextureMagnificationFilter magFilter,
      TextureMinificationFilter minFilter,
      TextureWrapMode sWrapMode,
      TextureWrapMode tWrapMode,
      bool genMipmaps)
  {
    int stbWidth, stbHeight, stbChannels;
    stbi_uc* data = stbi_load(filePath.c_str(), &stbWidth, &stbHeight, &stbChannels, 0);

    if (!data)
    {
      LOTUS_LOG_ERROR("[Texture Error] Image without data at path {0}", filePath);
      LOTUS_ASSERT(false, "Exiting");
      stbi_image_free(data);
    }

    TextureConfig textureConfig;
    textureConfig.data = data;
    textureConfig.width = stbWidth;
    textureConfig.height = stbHeight;

    if (stbChannels == 1)
    {
      textureConfig.format = TextureFormat::RUnsigned;
    }
    else if (stbChannels == 4)
    {
      textureConfig.format = TextureFormat::RGBAUnsigned;
    }
    else if (stbChannels == 3)
    {
      textureConfig.format = TextureFormat::RGBUnsigned;
    }
    else
    {
      LOTUS_LOG_ERROR("[Texture Error] Invalid image format at path {0}", filePath);
      LOTUS_ASSERT(false, "Exiting");
    }

    textureConfig.magFilter = magFilter;
    textureConfig.minFilter = minFilter;
    textureConfig.sWrapMode = sWrapMode;
    textureConfig.tWrapMode = tWrapMode;
    
    GPUTexture* gpuTexture = new GPUTexture(textureConfig);

    stbi_image_free(data);
    
    return gpuTexture;
  }

  std::shared_ptr<GPUTexture> TextureLoader::loadTexture(
      const std::filesystem::path& filePath,
      TextureMagnificationFilter magFilter,
      TextureMinificationFilter minFilter,
      TextureWrapMode sWrapMode,
      TextureWrapMode tWrapMode,
      bool genMipmaps) noexcept
  {
    const std::string stringPath = filePath.string();

    // In case there already existed a loaded texture with the given path referenced by the textures map
    // it is returned immediately
    auto it = textureMap.find(stringPath);

    if (it != textureMap.end())
    {
      return it->second;
    }
    
    GPUTexture* texture = loadImageTexture(stringPath, magFilter, minFilter, sWrapMode, tWrapMode, genMipmaps);
    std::shared_ptr<GPUTexture> textureSharedPtr = std::shared_ptr<GPUTexture>(texture);

    // Before returning the loaded texture, we add it to the map so future loads are faster
    textureMap.insert({ stringPath, textureSharedPtr });
    return textureSharedPtr;
  }

}