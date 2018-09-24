#pragma once

#include "animation.h"

namespace Impacto {

class Character3D;

struct BoneTrackStatus {
  uint16_t CurrentKeys[BKT_Count];
  uint16_t NextKeys[BKT_Count];
};

struct MeshTrackStatus {
  uint16_t CurrentKeys[MKT_Count];
  uint16_t NextKeys[MKT_Count];
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
  MeshTrackStatus MeshKeys[ModelMaxMeshesPerModel];

  float LoopStart = 0.0f;
  float LoopEnd = 0.0f;
  bool IsPlaying = false;

  bool Tweening = true;

  void Unload();
  void Start(uint16_t animId);
  void Reset();
  void Update(float dt);
  void Seek(float dt);
};
}  // namespace Impacto