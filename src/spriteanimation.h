#pragma once

#include "spritesheet.h"
#include "animation.h"

namespace Impacto {

class SpriteAnimationDef;

struct SpriteAnimation : public Animation {
  SpriteAnimationDef* Def = 0;

  bool Exists();
  Sprite CurrentSprite();
};

class SpriteAnimationDef {
 public:
  float Duration;
  int FrameCount;
  Sprite* Frames;

  SpriteAnimation Instantiate();
};

}  // namespace Impacto