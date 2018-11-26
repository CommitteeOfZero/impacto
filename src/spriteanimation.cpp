#include "spriteanimation.h"

namespace Impacto {

bool SpriteAnimation::Exists() { return Def != 0; }
void SpriteAnimation::Reset() { Time = 0.0f; }
void SpriteAnimation::Update(float dt) {
  Time += dt;
  if (Time >= Def->Duration) {
    Time = fmodf(Time, Def->Duration);
  }
}
Sprite SpriteAnimation::CurrentSprite() {
  int frame = (int)(Time / Def->Duration * (float)Def->FrameCount);
  return Def->Frames[frame];
}
SpriteAnimation SpriteAnimationDef::Instantiate() {
  SpriteAnimation result;
  result.Def = this;
  result.Time = 0.0f;
  return result;
}
}  // namespace Impacto