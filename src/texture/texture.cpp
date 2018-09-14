#include "texture.h"

#include <string.h>

#include <GL/glew.h>

#include "../log.h"

namespace Impacto {

void Texture::Load1x1() {
  Width = 1;
  Height = 1;
  Format = TexFmt_RGBA;
  BufferSize = 4;
  Buffer = (uint8_t*)malloc(4);
  Buffer[0] = 0xFF;
  Buffer[1] = 0;
  Buffer[2] = 0xFF;
  Buffer[3] = 0xFF;
}

uint32_t Texture::Submit() {
  ImpLog(LL_Debug, LC_Render, "Submitting texture\n");

  uint32_t result;
  glGenTextures(1, &result);
  glBindTexture(GL_TEXTURE_2D, result);

  // Anisotropic filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

  // Load in data
  GLuint texFormat = Format == TexFmt_RGBA ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, texFormat, Width, Height, 0, texFormat,
               GL_UNSIGNED_BYTE, Buffer);

  // Build mip chain
  glGenerateMipmap(GL_TEXTURE_2D);

  return result;
}

}  // namespace Impacto