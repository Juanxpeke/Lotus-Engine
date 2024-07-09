#include "texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Lotus
{
  namespace Traditional
  {
    GLenum wrapEnumToOpenGLEnum(WrapMode wrapMode)
    {
      switch (wrapMode)
      {
        case WrapMode::Repeat:
          return GL_REPEAT;
          break;
        case WrapMode::ClampToEdge:
          return GL_CLAMP_TO_EDGE;
          break;
        case WrapMode::MirroredRepeat:
          return GL_MIRRORED_REPEAT;
          break;
        default:
          return GL_REPEAT;
          break;
      }
    }

    GLenum magnificationFilterEnumToOpenGLEnum(TextureMagnificationFilter filter)
    {
      switch (filter)
      {
        case TextureMagnificationFilter::Nearest:
          return GL_NEAREST;
          break;
        case TextureMagnificationFilter::Linear:
          return GL_LINEAR;
          break;
        default:
          return GL_LINEAR;
          break;
      }
    }

    GLenum minificationFilterEnumToOpenGLEnum(TextureMinificationFilter filter)
    {
      switch (filter)
      {
        case TextureMinificationFilter::Nearest:
          return GL_NEAREST;
          break;
        case TextureMinificationFilter::Linear:
          return GL_LINEAR;
          break;
        case TextureMinificationFilter::NearestMipmapNearest:
          return GL_NEAREST_MIPMAP_NEAREST;
          break;
        case TextureMinificationFilter::NearestMipmapLinear:
          return GL_NEAREST_MIPMAP_LINEAR;
          break;
        case TextureMinificationFilter::LinearMipmapNearest:
          return GL_LINEAR_MIPMAP_NEAREST;
          break;
        case TextureMinificationFilter::LinearMipmapLinear:
          return GL_LINEAR_MIPMAP_LINEAR;
          break;
        default:
          return GL_NEAREST_MIPMAP_LINEAR;
          break;
      }
    }

    Texture::Texture(const std::string& stringFilePath,
        TextureMagnificationFilter magFilter,
        TextureMinificationFilter minFilter,
        WrapMode sWrapMode,
        WrapMode tWrapMode,
        bool genMipmaps) :
      ID(0),
      width(0),
      height(0),
      channels(0)
    {
      int stbWidth, stbHeight, stbChannels;
      stbi_uc* data = stbi_load(stringFilePath.c_str(), &stbWidth, &stbHeight, &stbChannels, 0);
      
      if (!data)
      {
        // LOG_ERROR("Texture Error: Failed to load texture from {0} file.", stringFilePath);
        stbi_image_free(data);
        return;
      }

      GLenum internalFormat = 0;
      GLenum dataFormat = 0;
      
      if (stbChannels == 1)
      {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
      }
      else if (stbChannels == 4)
      {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
      }
      else if (stbChannels == 3)
      {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
      }

      if (!(internalFormat & dataFormat))
      {
        // LOG_ERROR("Texture Error: Texture format not supported.", stringFilePath);
        stbi_image_free(data);
        return;
      }

      glCreateTextures(GL_TEXTURE_2D, 1, &ID);
      glTextureStorage2D(ID, 1, internalFormat, stbWidth, stbHeight);
      glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrapEnumToOpenGLEnum(sWrapMode));
      glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrapEnumToOpenGLEnum(tWrapMode));
      glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, magnificationFilterEnumToOpenGLEnum(magFilter));
      glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, minificationFilterEnumToOpenGLEnum(minFilter));
      glTextureSubImage2D(ID, 0, 0, 0, stbWidth, stbHeight, dataFormat, GL_UNSIGNED_BYTE, data);
      
      if (genMipmaps)
      {
        glGenerateTextureMipmap(ID);
      }
      
      channels = stbChannels;
      width = stbWidth;
      height = stbHeight;
      
      stbi_image_free(data);
    }

    Texture::~Texture()
    {
      if (ID)
        clearData();
    }

    void Texture::setSWrapMode(WrapMode wrapMode) noexcept
    {
      glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrapEnumToOpenGLEnum(wrapMode));
    }

    void Texture::setTWrapMode(WrapMode wrapMode) noexcept
    {
      glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrapEnumToOpenGLEnum(wrapMode));
    }

    void Texture::setMagnificationFilter(TextureMagnificationFilter magFilter) noexcept
    {
      glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, magnificationFilterEnumToOpenGLEnum(magFilter));
    }

    void Texture::setMinificationFilter(TextureMinificationFilter minFilter) noexcept
    {
      glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, minificationFilterEnumToOpenGLEnum(minFilter));
    }

    void Texture::clearData() noexcept
    {
      //ASSERT(m_ID, "Texture Error: Trying to clear data from already freed texture.");
      glDeleteTextures(1, &ID);
      ID = 0;
    }    
  }
}
