#pragma once

#include "../impacto.h"

namespace Impacto {

enum TexFmt { TexFmt_RGB, TexFmt_RGBA, TexFmt_U8 };

struct Texture {
  int Width;
  int Height;
  TexFmt Format;
  uint8_t* Buffer;
  int BufferSize;

  void Init(TexFmt fmt, int width, int height);

  bool Load(SDL_RWops* stream);
  void Load1x1(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0,
               uint8_t alpha = 0);
  void LoadCheckerboard();
  void LoadPoliticalCompass();
  uint32_t Submit();
};

}  // namespace Impacto