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

  void Load1x1();
  void LoadPoliticalCompass();
  uint32_t Submit();
};

}  // namespace Impacto