#pragma once

#include <cstddef>
#include <cstdint>

namespace Lotus
{
  enum class TextureFormat
  {
    Invalid,
    Red,
    RGB,
    RGBA
  };

  enum class TextureDataType
  {
    UnsignedByte,
    Float
  };
  
  enum class TextureMagnificationFilter
  {
    Nearest,
    Linear
  };

  enum class TextureMinificationFilter
  {
    Nearest,
    Linear,
    NearestMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapNearest,
    LinearMipmapLinear
  };

  enum class TextureWrapMode
  {
    Repeat,
    ClampToEdge,
    MirroredRepeat
  };

  struct TextureConfig
  {
    uint32_t width;
    uint32_t height;

    const void* data = nullptr;
    size_t dataSize = 0;

    TextureFormat format = TextureFormat::Invalid;
    TextureDataType dataType = TextureDataType::UnsignedByte;
    TextureMagnificationFilter magFilter = TextureMagnificationFilter::Linear;
    TextureMinificationFilter minFilter = TextureMinificationFilter::LinearMipmapLinear;
    TextureWrapMode sWrapMode = TextureWrapMode::Repeat;
    TextureWrapMode tWrapMode = TextureWrapMode::Repeat;

    bool genMipmaps = false;
  };

  class GPUTexture
  {
  public:
    GPUTexture(TextureConfig textureConfig);
    ~GPUTexture();
    
    uint32_t getID() const { return ID; }
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }

    void setSWrapMode(TextureWrapMode wrapMode) noexcept;
    void setTWrapMode(TextureWrapMode wrapMode) noexcept;
    void setMagnificationFilter(TextureMagnificationFilter magFilter) noexcept;
    void setMinificationFilter(TextureMinificationFilter minFilter) noexcept;

  private:
    uint32_t ID;
    uint32_t width;
    uint32_t height;
  };

}
