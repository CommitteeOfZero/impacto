#pragma once

#include <SDL_rwops.h>

#include "model.h"

namespace Impacto {

int const AnimMaxMorphTargetsPerTrack = 16;

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

// Note: Unlike R;NE animation files, we keep a track per mesh, not a track per
// mesh group
struct MeshTrack {
  uint16_t Mesh;

  int VisibilityOffset;
  int MorphInfluenceOffsets[AnimMaxMorphTargetsPerTrack];

  uint16_t VisibilityCount;

  uint16_t MorphTargetCount;
  // Unlike R;NE animation files, these IDs go into Model->MorphTargets, not
  // Mesh->MorphTargetIds
  uint16_t MorphTargetIds[AnimMaxMorphTargetsPerTrack];
  uint16_t MorphInfluenceCounts[AnimMaxMorphTargetsPerTrack];
};

class Animation {
 public:
  static Animation* Load(SDL_RWops* stream, Model* model, uint16_t id);
  ~Animation();

  // Per-model ID
  uint16_t Id = 0;

  // Whole animation duration in seconds
  float Duration = 0.0f;

  int CoordKeyframeCount = 0;
  CoordKeyframe* CoordKeyframes = 0;

  int QuatKeyframeCount = 0;
  QuatKeyframe* QuatKeyframes = 0;

  int BoneTrackCount = 0;
  BoneTrack BoneTracks[ModelMaxBonesPerModel];

  int MeshTrackCount = 0;
  MeshTrack MeshTracks[ModelMaxMeshesPerModel];
};

}  // namespace Impacto