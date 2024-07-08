#pragma once

#include <string>
#include "../../math/types.h"

namespace Lotus
{
  namespace Traditional
  {
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

    enum class WrapMode
    {
      Repeat,
      ClampToEdge,
      MirroredRepeat
    };

    class Texture
    {
    friend class TextureManager;

    public:
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
  }
}