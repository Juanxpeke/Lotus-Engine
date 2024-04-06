#pragma once

#include <cstdint>
#include <string>

enum class TextureMagnificationFilter
{
  Nearest,
  Linear
};

enum class TextureMinificationFilter
{
  Nearest,
  Linear,
  NearestMimapNearest,
  NearestMipmapLinear,
  LinearMipmapNearest,
  LinearMipmapLinear
};

enum class WrapMode
{
  Repeat,
  ClampToEdge,
  MirroedRepeat
};

class Texture
{
public:
  // friend class TextureManager;
  Texture(const std::string& stringFilePath,
      TextureMagnificationFilter magFilter = TextureMagnificationFilter::Linear,
      TextureMinificationFilter minFilter = TextureMinificationFilter::LinearMipmapLinear,
      WrapMode sWrapMode = WrapMode::Repeat,
      WrapMode tWrapMode = WrapMode::Repeat,
      bool genMipmaps = false);
  ~Texture();
  
  uint32_t getWidth() const { return width; }
  uint32_t getHeight() const { return height; }
  uint32_t getID() const { return ID; }
  void setSWrapMode(WrapMode wrapMode) noexcept;
  void setTWrapMode(WrapMode wrapMode) noexcept;
  void setMagnificationFilter(TextureMagnificationFilter magFilter) noexcept;
  void setMinificationFilter(TextureMinificationFilter minFilter) noexcept;

private:
  void clearData() noexcept;

  uint32_t ID;
  uint32_t width;
  uint32_t height;
  uint32_t channels;
};