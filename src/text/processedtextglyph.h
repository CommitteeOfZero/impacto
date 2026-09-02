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
  glm::vec2 Position;

  void Move(glm::vec2 offset) {
    Position += offset;
    DestRect += offset;
  }
  void MoveTo(glm::vec2 position) { Move(position - Position); }
};

}  // namespace Impacto
