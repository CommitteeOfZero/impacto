#include "texture.h"

#include <string.h>

#include <glad/glad.h>

#include "../log.h"
#include "gxtloader.h"
#include "plainloader.h"

namespace Impacto {

bool Texture::Load(SDL_RWops* stream) {
  using namespace TexLoad;

  if (TextureIsGXT(stream)) return TextureLoadGXT(stream, this);
  if (TextureIsPlain(stream)) return TextureLoadPlain(stream, this);

  uint32_t magic = SDL_ReadBE32(stream);
  ImpLog(LL_Error, LC_TextureLoad,
         "No loader for texture, possible magic %08X\n", magic);
  return false;
}

void Texture::Init(TexFmt fmt, int width, int height) {
  Width = width;
  Height = height;
  Format = fmt;

  switch (fmt) {
    case TexFmt_RGBA:
      BufferSize = width * height * 4;
      break;
    case TexFmt_RGB:
      BufferSize = width * height * 3;
      break;
    case TexFmt_U8:
      BufferSize = width * height;
      break;
  }
  Buffer = (uint8_t*)malloc(BufferSize);
}

void Texture::Load1x1() {
  Init(TexFmt_RGBA, 1, 1);
  *(uint32_t*)Buffer = 0;
}

void Texture::LoadPoliticalCompass() {
  Init(TexFmt_RGBA, 512, 512);

  for (int x = 0; x < Width / 2; x++) {
    for (int y = 0; y < Height / 2; y++) {
      Buffer[4 * x + 4 * y * Height + 0] = 0xE0;
      Buffer[4 * x + 4 * y * Height + 1] = 0x77;
      Buffer[4 * x + 4 * y * Height + 2] = 0x77;
      Buffer[4 * x + 4 * y * Height + 3] = 0xFF;

      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 0] = 0x38;
      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 1] = 0xBE;
      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 2] = 0xE0;
      Buffer[4 * (Width / 2 + x) + 4 * y * Height + 3] = 0xFF;

      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 0] = 0x89;
      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 1] = 0xC7;
      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 2] = 0x72;
      Buffer[4 * x + 4 * (Height / 2 + y) * Height + 3] = 0xFF;

      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 0] = 0xC6;
      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 1] = 0x8D;
      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 2] = 0xC3;
      Buffer[4 * (Width / 2 + x) + 4 * (Height / 2 + y) * Height + 3] = 0xFF;
    }
  }
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
  GLuint texFormat;
  switch (Format) {
    case TexFmt_RGBA:
      texFormat = GL_RGBA;
      break;
    case TexFmt_RGB:
      texFormat = GL_RGB;
      break;
    case TexFmt_U8:
      texFormat = GL_RED;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, texFormat, Width, Height, 0, texFormat,
               GL_UNSIGNED_BYTE, Buffer);

  // Build mip chain
  // TODO do this ourselves outside of Submit(), this can easily cause a
  // framedrop
  glGenerateMipmap(GL_TEXTURE_2D);

  // TODO I meant to do this elsewhere but we gotta do it somewhere
  free(Buffer);

  return result;
}

}  // namespace Impacto