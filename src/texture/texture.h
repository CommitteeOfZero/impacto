#pragma once

#include "../impacto.h"

namespace Impacto {

enum TexFmt {
  TexFmt_RGB,
  TexFmt_RGBA,
};

struct Texture {
  int Width;
  int Height;
  TexFmt Format;
  uint8_t* Buffer;
  int BufferSize;

  void Load1x1();
  uint32_t Submit();
};

}  // namespace Impacto