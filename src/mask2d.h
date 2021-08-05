#pragma once

#include "renderer2d.h"
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

int const MaxMasks2D = 32;

extern Mask2D Masks2D[MaxMasks2D];

}  // namespace Impacto