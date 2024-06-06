#pragma once

#include "../../io/stream.h"

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

enum BoneKeyType {
  BKT_TranslateX = 0,
  BKT_TranslateY = 1,
  BKT_TranslateZ = 2,
  BKT_Rotate = 3,
  BKT_ScaleX = 4,
  BKT_ScaleY = 5,
  BKT_ScaleZ = 6,
  BKT_Count = 7
};

enum MeshKeyType {
  MKT_Visible = 0,
  MKT_MorphInfluenceStart = 1,
  MKT_Count = MKT_MorphInfluenceStart + AnimMaxMorphTargetsPerTrack
};

struct BoneTrack {
  // DaSH addition
  uint8_t Name[32];

  uint16_t Bone;

  int KeyOffsets[BKT_Count];
  uint16_t KeyCounts[BKT_Count];
};

// Note: Unlike R;NE animation files, we keep a track per mesh, not a track per
// mesh group
struct MeshTrack {
  // DaSH addition
  uint8_t Name[32];

  uint16_t Mesh;

  int KeyOffsets[MKT_Count];
  uint16_t KeyCounts[MKT_Count];

  uint16_t MorphTargetCount;
  uint16_t MorphTargetIds[AnimMaxMorphTargetsPerTrack];
};

class ModelAnimation {
 public:
  static ModelAnimation* Load(Io::Stream* stream, Model* model, int16_t animId);
  ~ModelAnimation();

  // Per-model ID
  // or global in DaSH
  int16_t Id = 0;

  std::string Name;

  // Whole animation duration in seconds
  float Duration = 0.0f;

  bool OneShot = false;
  float LoopStart = 0.0f;
  float LoopEnd = 0.0f;

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