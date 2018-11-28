#include "spriteanimation.h"

namespace Impacto {

bool SpriteAnimation::Exists() { return Def != 0; }
Sprite SpriteAnimation::CurrentSprite() {
  int frame = (int)(Progress * (float)Def->FrameCount);
  return Def->Frames[frame];
}
SpriteAnimation SpriteAnimationDef::Instantiate() {
  SpriteAnimation result;
  result.Def = this;
  result.DurationIn = this->Duration;
  result.DurationOut = this->Duration;
  return result;
}
}  // namespace Impacto