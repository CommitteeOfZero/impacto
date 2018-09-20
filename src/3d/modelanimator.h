#pragma once

#include "animation.h"

namespace Impacto {

class Character3D;

struct BoneTrackStatus {
  uint16_t CurrentTranslateXKey = 0;
  uint16_t CurrentTranslateYKey = 0;
  uint16_t CurrentTranslateZKey = 0;

  uint16_t CurrentScaleXKey = 0;
  uint16_t CurrentScaleYKey = 0;
  uint16_t CurrentScaleZKey = 0;

  uint16_t CurrentRotateKey = 0;

  uint16_t NextTranslateXKey = 0;
  uint16_t NextTranslateYKey = 0;
  uint16_t NextTranslateZKey = 0;

  uint16_t NextScaleXKey = 0;
  uint16_t NextScaleYKey = 0;
  uint16_t NextScaleZKey = 0;

  uint16_t NextRotateKey = 0;
};

class ModelAnimator {
 public:
  // Note: These are has-a relationships - Character and CurrentAnimation should
  // not be deleted with the animator
  Character3D* Character = 0;
  Animation* CurrentAnimation = 0;

  // Time in seconds
  float CurrentTime = 0;
  BoneTrackStatus BoneKeys[ModelMaxBonesPerModel];

  bool Tweening = true;

  void Start(uint16_t animId);
  void Reset();
  void Update(float dt);
};
}  // namespace Impacto