#include "i_texture_manager.h"

#include <glad/glad.h>

std::shared_ptr<Texture> TextureManager::loadTexture(
    const std::filesystem::path& filePath,
    TextureMagnificationFilter magFilter,
    TextureMinificationFilter minFilter,
    WrapMode sWrapMode,
    WrapMode tWrapMode,
    bool genMipmaps) noexcept
{
  const std::string stringPath = filePath.string();

  // In case there already existed a loaded texture with the given path referenced by the textures map
  // it is returned immediately
  auto it = textureMap.find(stringPath);
  if (it != textureMap.end())
    return it->second;

  Texture* texturePtr = new Texture(stringPath, magFilter, minFilter, sWrapMode, tWrapMode, genMipmaps);
  std::shared_ptr<Texture> textureSharedPtr = std::shared_ptr<Texture>(texturePtr);

  // Before returning the loaded texture, we add it to the map so future loads are faster
  textureMap.insert({ stringPath, textureSharedPtr });

  return textureSharedPtr;
}

void TextureManager::cleanUnusedTextures() noexcept
{
  for (auto i = textureMap.begin(), last = textureMap.end(); i != last;)
  {
    if (i->second.use_count() == 1) {
      i = textureMap.erase(i);
    }
    else {
      ++i;
    }
  }
}

void TextureManager::shutDown() noexcept
{
  for (auto& entry : textureMap) {
    (entry.second)->clearData();
  }
  
  textureMap.clear();
}
