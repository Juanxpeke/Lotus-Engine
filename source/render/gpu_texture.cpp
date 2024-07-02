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
      case TextureFormat::RUnsigned:
        return GL_R8;
      case TextureFormat::RFloat:
        return GL_R32F;
      case TextureFormat::RGBUnsigned:
        return GL_RGB8;
      case TextureFormat::RGBFloat:
        return GL_RGB32F;
      case TextureFormat::RGBAUnsigned:
        return GL_RGBA8;
      case TextureFormat::RGBAFloat:
        return GL_RGBA32F;
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
      case TextureFormat::RUnsigned:
      case TextureFormat::RFloat:
        return GL_RED;
      case TextureFormat::RGBUnsigned:
      case TextureFormat::RGBFloat:
        return GL_RGB;
      case TextureFormat::RGBAUnsigned:
      case TextureFormat::RGBAFloat:
        return GL_RGBA;
      default:
        return GL_RGB;
    }

    return GL_INVALID_ENUM;
  }

  GLenum dataTypeEnumToOpenGLEnum(TextureFormat format)
  {
    switch(format)
    {
      case TextureFormat::Invalid:
        break;
      case TextureFormat::RUnsigned:
      case TextureFormat::RGBUnsigned:
      case TextureFormat::RGBAUnsigned:
        return GL_UNSIGNED_BYTE;
      case TextureFormat::RFloat:
      case TextureFormat::RGBFloat:
      case TextureFormat::RGBAFloat:
        return GL_FLOAT;
      default:
        return GL_UNSIGNED_BYTE; 
    }

    return GL_INVALID_ENUM;
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
    height(textureConfig.height),
    format(textureConfig.format)
  {
    GLenum internalFormat = internalFormatEnumToOpenGLEnum(format);
    GLenum dataFormat = dataFormatEnumToOpenGLEnum(format);
    GLenum dataType = dataTypeEnumToOpenGLEnum(format);

    glCreateTextures(GL_TEXTURE_2D, 1, &ID);
    glTextureStorage2D(ID, 1, internalFormat, width, height);

    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrapEnumToOpenGLEnum(textureConfig.sWrapMode));
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrapEnumToOpenGLEnum(textureConfig.tWrapMode));
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, magnificationFilterEnumToOpenGLEnum(textureConfig.magFilter));
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, minificationFilterEnumToOpenGLEnum(textureConfig.minFilter));
    
    if (textureConfig.data)
    {
      glTextureSubImage2D(ID, 0, 0, 0, width, height, dataFormat, dataType, textureConfig.data);
    }
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
      LOTUS_LOG_INFO("[Texture Log] Deleted GPU texture with ID {0}", ID);
      
      glDeleteTextures(1, &ID);
      ID = 0;
    }
    else
    {
      LOTUS_LOG_ERROR("[Texture Error] Tried to delete already deleted texture");
    }
  }

  void GPUTexture::setData(const void* data)
  {
    GLenum dataFormat = dataFormatEnumToOpenGLEnum(format);
    GLenum dataType = dataTypeEnumToOpenGLEnum(format);

    glTextureSubImage2D(ID, 0, 0, 0, width, height, dataFormat, dataType, data);
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


  GPUArrayTexture::GPUArrayTexture(TextureConfig textureConfig) :
    ID(0),
    width(textureConfig.width),
    height(textureConfig.height),
    layers(textureConfig.depth),
    format(textureConfig.format)
  {
    GLenum internalFormat = internalFormatEnumToOpenGLEnum(format);
    GLenum dataFormat = dataFormatEnumToOpenGLEnum(format);
    GLenum dataType = dataTypeEnumToOpenGLEnum(format);

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &ID);
    glTextureStorage3D(ID, 1, internalFormat, width, height, layers);

    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrapEnumToOpenGLEnum(textureConfig.sWrapMode));
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrapEnumToOpenGLEnum(textureConfig.tWrapMode));
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, magnificationFilterEnumToOpenGLEnum(textureConfig.magFilter));
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, minificationFilterEnumToOpenGLEnum(textureConfig.minFilter));
    
    if (textureConfig.data)
    {
      glTextureSubImage3D(ID, 0, 0, 0, 0, width, height, layers, dataFormat, dataType, textureConfig.data);
    }
    if (textureConfig.genMipmaps)
    {
      glGenerateTextureMipmap(ID);
    }

    LOTUS_LOG_INFO("[Texture Log] Created GPU texture array with ID {0}", ID);
  }

  GPUArrayTexture::~GPUArrayTexture()
  {
    if (ID)
    {
      LOTUS_LOG_INFO("[Texture Log] Deleted GPU texture array with ID {0}", ID);
      
      glDeleteTextures(1, &ID);
      ID = 0;
    }
    else
    {
      LOTUS_LOG_ERROR("[Texture Error] Tried to delete already deleted texture array");
    }
  }

  void GPUArrayTexture::setLayerData(uint16_t layer, const void* data)
  {
    GLenum dataFormat = dataFormatEnumToOpenGLEnum(format);
    GLenum dataType = dataTypeEnumToOpenGLEnum(format);

    glTextureSubImage3D(ID, 0, 0, 0, layer, width, height, 1, dataFormat, dataType, data);
  }

  void GPUArrayTexture::setSWrapMode(TextureWrapMode wrapMode) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, wrapEnumToOpenGLEnum(wrapMode));
  }

  void GPUArrayTexture::setTWrapMode(TextureWrapMode wrapMode) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, wrapEnumToOpenGLEnum(wrapMode));
  }

  void GPUArrayTexture::setMagnificationFilter(TextureMagnificationFilter magFilter) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, magnificationFilterEnumToOpenGLEnum(magFilter));
  }

  void GPUArrayTexture::setMinificationFilter(TextureMinificationFilter minFilter) noexcept
  {
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, minificationFilterEnumToOpenGLEnum(minFilter));
  }

}