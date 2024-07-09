#pragma once

#include <set>
#include <string>
#include <glad/glad.h>

namespace Lotus
{
  enum class OpenGLExtension
  {
    BindlessTexture,
    SparseTexture,
    SparseTextureClamp,
    AMDPinnedMemory
  };

  class OpenGLExtensionChecker
  {
  public:
    static bool isExtensionSupported(OpenGLExtension extension)
    {
      return isExtensionSupported(OpenGLExtensionToString(extension));
    }
  
    static bool isExtensionSupported(const std::string& extension)
    {
      if (supportedExtensions.empty())
      {
        GLint nExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
        
        for (GLint i = 0; i < nExtensions; ++i)
        {
          const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
          supportedExtensions.insert(std::string(ext));
        }
      }

      return supportedExtensions.find(extension) != supportedExtensions.end();
    }

  private:
    static std::string OpenGLExtensionToString(OpenGLExtension extension)
    {
      switch (extension)
      {
        case OpenGLExtension::BindlessTexture:
          return "GL_ARB_bindless_texture";
        case OpenGLExtension::SparseTexture:
          return "GL_ARB_sparse_texture";
        case OpenGLExtension::SparseTextureClamp:
          return "GL_ARB_sparse_texture_clamp";
        case OpenGLExtension::AMDPinnedMemory:
          return "GL_AMD_pinned_memory";
        default:
          return "INVALID_EXTENSION";
      }
    }

    static std::set<std::string> supportedExtensions;
  };

  std::set<std::string> OpenGLExtensionChecker::supportedExtensions;
}