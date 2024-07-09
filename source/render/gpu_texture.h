#pragma once

#include "../math/types.h"

namespace Lotus
{
  enum class TextureFormat
  {
    Invalid,
    RUnsigned,
    RFloat,
    RGBUnsigned,
    RGBFloat,
    RGBAUnsigned,
    RGBAFloat
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
    uint32_t depth;

    const void* data = nullptr;
    size_t dataSize = 0;

    TextureFormat format = TextureFormat::Invalid;
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
    
    GPUTexture& operator=(const GPUTexture& other) = delete;

    uint32_t getID() const { return ID; }
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
    uint64_t getHandle() const { return handle; }

    void setData(const void* data);

    void setSWrapMode(TextureWrapMode wrapMode) noexcept;
    void setTWrapMode(TextureWrapMode wrapMode) noexcept;
    void setMagnificationFilter(TextureMagnificationFilter magFilter) noexcept;
    void setMinificationFilter(TextureMinificationFilter minFilter) noexcept;

  private:
    uint32_t ID;
    uint64_t handle;
    uint32_t width;
    uint32_t height;

    const TextureFormat format;
  };

  class GPUArrayTexture
  {
  public:
    GPUArrayTexture(TextureConfig textureConfig);
    ~GPUArrayTexture();
    
    GPUArrayTexture& operator=(const GPUArrayTexture& other) = delete;

    uint32_t getID() const { return ID; }
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
    uint32_t getLayers() const { return layers; }

    void setLayerData(uint16_t layer, const void* data);

    void setSWrapMode(TextureWrapMode wrapMode) noexcept;
    void setTWrapMode(TextureWrapMode wrapMode) noexcept;
    void setMagnificationFilter(TextureMagnificationFilter magFilter) noexcept;
    void setMinificationFilter(TextureMinificationFilter minFilter) noexcept;

  private:
    uint32_t ID;
    uint32_t width;
    uint32_t height;
    uint16_t layers;

    const TextureFormat format;
  };

}
