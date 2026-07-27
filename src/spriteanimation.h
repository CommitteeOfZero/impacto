#pragma once

#include "spritesheet.h"
#include "animation.h"

namespace Impacto {

class SpriteAnimationDef;

class SpriteAnimation final : public Animation {
 public:
  SpriteAnimation(const SpriteAnimationDef& def) : Def(def) {}

  Sprite CurrentSprite() const;

  std::reference_wrapper<const SpriteAnimationDef> Def;
};

class SpriteAnimationDef {
 public:
  float Duration;
  std::vector<Sprite> Frames;

  SpriteAnimation Instantiate() const;
};

class FixedSpriteAnimationDef;

/*
  Sprite animation that has a separate in and out animation
  Consists of one contiguous Sprite array, with an int FixedFrameIdx
  denoting the 0-indexed id of the last frame of the in animation.

  If the animation is out and gets told to move in, it will play
  the in animation to completion.
  Similarly, if the animation is in and gets told to move out, it will
  play the out animation to completion.
  If either the in or out animation is playing, and it suddenly gets told
  to do the other animation, it will finish the currently playing animation
  and then immediately move on to the requested animation.

  The progress is split up such that the interval
  [0, 1 - FixedFrameIdx / FrameCount] is the out animation (with 0 being fully
  out), and the interval [1 - FixedFrameIdx / FrameCount, 1] is the in animation
  (with 1 being fully in).
*/
class FixedSpriteAnimation final : public Animation {
 public:
  FixedSpriteAnimation(const FixedSpriteAnimationDef& def) : Def(def) {}

  Sprite CurrentSprite() const;
  float GetFixedSpriteProgress() const;

  void StartInImpl(bool reset) override {
    if (reset) Progress = GetFixedSpriteProgress();
  }
  void StartOutImpl(bool reset) override {
    if (reset) Progress = GetFixedSpriteProgress();
  }
  void UpdateImpl(float dt) override;

  std::reference_wrapper<const FixedSpriteAnimationDef> Def;
};

class FixedSpriteAnimationDef {
 public:
  float Duration;
  std::vector<Sprite> Frames;
  size_t FixedFrameIdx;

  FixedSpriteAnimation Instantiate() const;
};

}  // namespace Impacto
