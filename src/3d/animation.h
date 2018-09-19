#pragma once

#include <SDL_rwops.h>

#include "model.h"

namespace Impacto {

struct CoordKeyframe {
  float Time;
  float Value;
};

struct QuatKeyframe {
  float Time;
  glm::quat Value;
};

struct BoneTrack {
  uint16_t Bone;

  int TranslateXOffset;
  int TranslateYOffset;
  int TranslateZOffset;
  int ScaleXOffset;
  int ScaleYOffset;
  int ScaleZOffset;
  int RotateOffset;

  uint16_t TranslateXCount;
  uint16_t TranslateYCount;
  uint16_t TranslateZCount;
  uint16_t ScaleXCount;
  uint16_t ScaleYCount;
  uint16_t ScaleZCount;
  uint16_t RotateCount;
};

class Animation {
 public:
  static Animation* Load(SDL_RWops* stream, Model* model);
  ~Animation();

  float Duration = 0.0f;

  int CoordKeyframeCount = 0;
  CoordKeyframe* CoordKeyframes = 0;

  int QuatKeyframeCount = 0;
  QuatKeyframe* QuatKeyframes = 0;

  int BoneTrackCount = 0;
  BoneTrack BoneTracks[ModelMaxBonesPerModel];
};

}  // namespace Impacto