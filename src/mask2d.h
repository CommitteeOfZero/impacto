#pragma once

#include "texture/texture.h"
#include "spritesheet.h"

namespace Impacto {

class Mask2D {
 public:
  static void Init();

  Sprite MaskSprite;
  bool LoadSync(uint32_t maskId);
  void UnloadSync();

 private:
  Texture MaskTexture;
  SpriteSheet MaskSpriteSheet;
};

int constexpr MaxMasks2D = 32;

inline Mask2D Masks2D[MaxMasks2D];

}  // namespace Impacto