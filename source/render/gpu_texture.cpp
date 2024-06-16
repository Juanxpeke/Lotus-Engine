#include "gpu_texture.h"

#include <glad/glad.h>
#include "../util/log.h"

namespace Lotus
{
  GLenum internalFormatEnumToOpenGLEnum(TextureFormat format)
  {
    switch(format)
    {
      case TextureFormat::Invalid:
        break;
      case TextureFormat::Red:
        return GL_R8;
      case TextureFormat::RGB:
        return GL_RGB8;
      case TextureFormat::RGBA:
        return GL_RGBA8;
      default:
        return GL_RGB8;
    }
     
    return GL_INVALID_ENUM;
  }

  GLenum dataFormatEnumToOpenGLEnum(TextureFormat format)
  {
    switch(format)
    {
      case TextureFormat::Invalid:
        break;
      case TextureFormat::Red:
        return GL_RED;
      case TextureFormat::RGB:
        return GL_RGB;
      case TextureFormat::RGBA:
        return GL_RGBA;
      default:
        return GL_RGB;
    }

    return GL_INVALID_ENUM;
  }

  GLenum dataTypeEnumToOpenGLEnum(TextureDataType type)
  {
    switch(type)
    {
      case TextureDataType::UnsignedByte:
        return GL_UNSIGNED_BYTE;
      case TextureDataType::Float:
        return GL_FLOAT;
      default:
        return GL_UNSIGNED_BYTE; 
    }
  }

  GLenum wrapEnumToOpenGLEnum(TextureWrapMode wrapMode)
  {
    switch (wrapMode)
    {
      case TextureWrapMode::Repeat:
        return GL_REPEAT;
      case TextureWrapMode::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
      case TextureWrapMode::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
      default:
        return GL_REPEAT;
    }
  }

  GLenum magnificationFilterEnumToOpenGLEnum(TextureMagnificationFilter filter)
  {
    switch (filter)
    {
      case TextureMagnificationFilter::Nearest:
        return GL_NEAREST;
      case TextureMagnificationFilter::Linear:
        return GL_LINEAR;
      default:
        return GL_LINEAR;
    }
  }

  GLenum minificationFilterEnumToOpenGLEnum(TextureMinificationFilter filter)
  {
    switch (filter)
    {
      case TextureMinificationFilter::Nearest:
        return GL_NEAREST;
      case TextureMinificationFilter::Linear:
        return GL_LINEAR;
      case TextureMinificationFilter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
      case TextureMinificationFilter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
      case TextureMinificationFilter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
      case TextureMinificationFilter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
      default:
        return GL_NEAREST_MIPMAP_LINEAR;
    }
  }

  GPUTexture::GPUTexture(TextureConfig textureConfig) :
    ID(0),
    width(textureConfig.width),
    height(textureConfig.height)
  {
    GLenum type = dataTypeEnumToOpenGLEnum(textureConfig.dataType);
    GLenum internalFormat = internalFormatEnumToOpenGLEnum(textureConfig.format);
    GLenum dataFormat = dataFormatEnumToOpenGLEnum(textureConfig.format);

    glCreateTextures(GL_TEXTURE_2D, 1, &ID);
    glTextureStorage2D(ID, 1, internalFormat, textureConfig.width, textureConfig.height);
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrapEnumToOpenGLEnum(textureConfig.sWrapMode));
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrapEnumToOpenGLEnum(textureConfig.tWrapMode));
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, magnificationFilterEnumToOpenGLEnum(textureConfig.magFilter));
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, minificationFilterEnumToOpenGLEnum(textureConfig.minFilter));
    glTextureSubImage2D(ID, 0, 0, 0, textureConfig.width, textureConfig.height, dataFormat, type, textureConfig.data);
    
    if (textureConfig.genMipmaps)
    {
      glGenerateTextureMipmap(ID);
    }

    LOTUS_LOG_INFO("[Texture Log] Created GPU texture with ID {0}", ID);
  }

  GPUTexture::~GPUTexture()
  {
    if (ID)
    {
      glDeleteTextures(1, &ID);
      ID = 0;
      LOTUS_LOG_INFO("[Texture Log] Deleted GPU texture with ID {0}", ID);
    }
    else
    {
      LOTUS_LOG_ERROR("[Texture Error] Tried to delete already deleted texture");
    }
  }

  void GPUTexture::setSWrapMode(TextureWrapMode wrapMode) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrapEnumToOpenGLEnum(wrapMode));
  }

  void GPUTexture::setTWrapMode(TextureWrapMode wrapMode) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrapEnumToOpenGLEnum(wrapMode));
  }

  void GPUTexture::setMagnificationFilter(TextureMagnificationFilter magFilter) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, magnificationFilterEnumToOpenGLEnum(magFilter));
  }

  void GPUTexture::setMinificationFilter(TextureMinificationFilter minFilter) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, minificationFilterEnumToOpenGLEnum(minFilter));
  }

}