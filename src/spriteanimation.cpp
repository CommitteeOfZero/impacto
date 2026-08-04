#include "spriteanimation.h"

namespace Impacto {

Sprite SpriteAnimation::CurrentSprite() const {
  const size_t frame = std::clamp<size_t>(
      static_cast<size_t>(Progress *
                          static_cast<float>(Def.get().Frames.size())),
      0, Def.get().Frames.size() - 1);
  return Def.get().Frames[frame];
}

SpriteAnimation SpriteAnimationDef::Instantiate() const {
  SpriteAnimation result(*this);
  result.SetDuration(Duration);

  return result;
}

void FixedSpriteAnimation::UpdateImpl(float dt) {
  const float fixedSpriteProgress = GetFixedSpriteProgress();
  const AnimationDirection animationRequest = Direction;

  if ((Progress == 1.0f && Direction == AnimationDirection::Out) ||
      (Progress == 0.0f && Direction == AnimationDirection::In)) {
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
  if (Direction == AnimationDirection::In) {
    Progress = (Progress - fixedSpriteProgress) / (1.0f - fixedSpriteProgress);
    dt /= 1.0f - fixedSpriteProgress;
  } else {
    Progress /= fixedSpriteProgress;
    dt /= fixedSpriteProgress;
  }

  Animation::UpdateImpl(dt);

  // Revert coordinate transformation and normalization
  if (Direction == AnimationDirection::In) {
    Progress = Progress * (1.0f - fixedSpriteProgress) + fixedSpriteProgress;
  } else {
    Progress *= fixedSpriteProgress;
  }

  // Start requested animation after already playing, non-requested, animation
  // has finished
  const bool progressAtExtremum = (Progress == 0.0f || Progress == 1.0f);
  if (animationRequest != Direction && progressAtExtremum) {
    Direction = animationRequest;
    State = AnimationState::Playing;
  }
}

Sprite FixedSpriteAnimation::CurrentSprite() const {
  const size_t frame = std::clamp<size_t>(
      [this]() -> size_t {
        const float fixedSpriteProgress = GetFixedSpriteProgress();

        if (Progress > fixedSpriteProgress) {  // In animation
          return static_cast<size_t>(
              (Progress - fixedSpriteProgress) *
              static_cast<float>(Def.get().Frames.size()));
        } else if (Progress < fixedSpriteProgress) {  // Out animation
          return static_cast<size_t>(
              (1.0f - Progress) * static_cast<float>(Def.get().Frames.size()));
        } else {  // Progress = fixedSpriteProgress
          return Def.get().FixedFrameIdx +
                 (Direction == AnimationDirection::Out);
        }
      }(),
      0, Def.get().Frames.size() - 1);

  return Def.get().Frames[frame];
}

float FixedSpriteAnimation::GetFixedSpriteProgress() const {
  /* Converse because in- and out directions are reversed
    between spritesheet and implementation */
  return 1.0f -
         static_cast<float>(Def.get().FixedFrameIdx) / Def.get().Frames.size();
};

FixedSpriteAnimation FixedSpriteAnimationDef::Instantiate() const {
  FixedSpriteAnimation result(*this);
  result.SetDuration(Duration);

  return result;
}

}  // namespace Impacto
