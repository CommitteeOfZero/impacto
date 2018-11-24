#pragma once

#include "spritesheet.h"

namespace Impacto {

struct SpriteAnimationDef;

struct SpriteAnimation {
  SpriteAnimationDef* Def = 0;
  float Time;

  bool Exists();
  void Reset();
  void Update(float dt);
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