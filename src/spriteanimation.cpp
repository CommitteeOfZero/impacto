#include "spriteanimation.h"

namespace Impacto {

bool SpriteAnimation::Exists() { return Def != 0; }

Sprite SpriteAnimation::CurrentSprite() {
  int frame = (int)(Progress * (float)Def->FrameCount);

  if (frame >= Def->FrameCount)
    frame = Def->FrameCount - 1;
  else if (frame < 0)
    frame = 0;

  return Def->Frames[frame];
}

void FixedSpriteAnimation::StartInImpl(bool reset) {
  if (reset) Progress = GetFixedSpriteProgress();
}

void FixedSpriteAnimation::StartOutImpl(bool reset) {
  if (reset) Progress = GetFixedSpriteProgress();
}

void FixedSpriteAnimation::UpdateImpl(float dt) {
  float fixedSpriteProgress = GetFixedSpriteProgress();
  AnimationDirection animationRequest = Direction;

  if (Progress == 1.0f && Direction == -1 ||
      Progress == 0.0f && Direction == 1) {
    Progress = fixedSpriteProgress;
  }

  // Always play both parts of the animation in the correct direction
  // (At the start of the function "Direction" only signifies the
  //  whether the in or out animation should be played; not the actual
  //  direction of the animation)
  if (Progress != fixedSpriteProgress) {
    Direction = Progress > fixedSpriteProgress ? AnimationDirection::In
                                               : AnimationDirection::Out;
  }

  // Coordinate transformation and normalization for AddDelta
  if (Direction == 1) {
    Progress = (Progress - fixedSpriteProgress) / (1.0f - fixedSpriteProgress);
    dt /= 1.0f - fixedSpriteProgress;
  } else {
    Progress /= fixedSpriteProgress;
    dt /= fixedSpriteProgress;
  }

  SpriteAnimation::UpdateImpl(dt);

  // Revert coordinate transformation and normalization
  if (Direction == 1)
    Progress = Progress * (1.0f - fixedSpriteProgress) + fixedSpriteProgress;
  else
    Progress *= fixedSpriteProgress;

  // Start requested animation after already playing, non-requested, animation
  // has finished
  bool progressAtExtremum = (Progress == 0.0f || Progress == 1.0f);
  if (animationRequest != Direction && progressAtExtremum) {
    Direction = animationRequest;
    State = AnimationState::Playing;
  }
}

Sprite FixedSpriteAnimation::CurrentSprite() {
  int frame;
  float fixedSpriteProgress = GetFixedSpriteProgress();

  if (Progress > fixedSpriteProgress)  // In animation
    frame = (int)((Progress - fixedSpriteProgress) * (float)Def->FrameCount);
  else if (Progress < fixedSpriteProgress)  // Out animation
    frame = (int)((1.0f - Progress) * (float)Def->FrameCount);
  else {  // Progress = fixedSpriteProgress
    frame = Def->FixSpriteId;
    if (Direction == -1) frame++;
  }

  if (frame >= Def->FrameCount)
    frame = Def->FrameCount - 1;
  else if (frame < 0)
    frame = 0;

  return Def->Frames[frame];
}

float FixedSpriteAnimation::GetFixedSpriteProgress() const {
  /* Converse because in- and out directions are reversed
    between spritesheet and implementation */
  return 1.0f - (float)Def->FixSpriteId / Def->FrameCount;
}

SpriteAnimation SpriteAnimationDef::Instantiate() {
  SpriteAnimation result;
  result.Def = this;
  result.DurationIn = this->Duration;
  result.DurationOut = this->Duration;

  if (this->FixSpriteId != 0) {
    result.Progress =
        static_cast<FixedSpriteAnimation&>(result).GetFixedSpriteProgress();
  }

  return result;
}

}  // namespace Impacto