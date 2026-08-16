#pragma once

#include "../util.h"

namespace Impacto {

struct DialogueColorPair {
  uint32_t TextColor;
  uint32_t OutlineColor;
};

struct ProcessedTextGlyph {
  DialogueColorPair Colors;
  uint32_t CharId;
  float Opacity;
  RectF DestRect;
};

}  // namespace Impacto
