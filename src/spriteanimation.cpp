#include "spriteanimation.h"

namespace Impacto {

bool SpriteAnimation::Exists() { return Def != 0; }
Sprite SpriteAnimation::CurrentSprite() {
  int frame = (int)(Progress * (float)Def->FrameCount);
  if (frame >= Def->FrameCount) frame = Def->FrameCount - 1;
  if (frame < 0) frame = 0;
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