#include "modelanimator.h"

#include "renderable3d.h"

namespace Impacto {

void ModelAnimator::Start(int16_t animId) {
  assert(Character != 0);

  if (Character->StaticModel->Animations.find(animId) ==
      Character->StaticModel->Animations.end()) {
    CurrentAnimation = 0;
    IsPlaying = false;
    return;
  }
  CurrentAnimation = Character->StaticModel->Animations.at(animId);

  CurrentTime = 0.0f;
  LoopStart = CurrentAnimation->LoopStart;
  LoopEnd = CurrentAnimation->LoopEnd;
  IsPlaying = true;
  OneShot = CurrentAnimation->OneShot;
  Reset();
  Update(0);
}
void ModelAnimator::Reset() {
  assert(Character != 0);
  assert(CurrentAnimation != 0);

  Character->ReloadDefaultBoneTransforms();
  Character->ReloadDefaultMeshAnimStatus();

  if (CurrentAnimation != 0) {
    for (int i = 0; i < CurrentAnimation->BoneTrackCount; i++) {
      for (int j = 0; j < BKT_Count; j++) {
        BoneKeys[i].CurrentKeys[j] = 0;
        BoneKeys[i].NextKeys[j] = 1;
      }
    }

    for (int i = 0; i < CurrentAnimation->MeshTrackCount; i++) {
      for (int j = 0; j < MKT_Count; j++) {
        MeshKeys[i].CurrentKeys[j] = 0;
        MeshKeys[i].NextKeys[j] = 1;
      }
    }
  }
}

void ModelAnimator::Unload() {
  Character->ReloadDefaultBoneTransforms();
  Character->ReloadDefaultMeshAnimStatus();
  CurrentAnimation = 0;
  CurrentTime = 0.0f;
  LoopStart = 0.0f;
  LoopEnd = 0.0f;
  IsPlaying = false;
}

// For each sub-track, if we're not on the last keyframe already, make sure the
// *next* keyframe is *after* the current time
#define TrackIncrementLoop(targetType, dataType, type)            \
  while (targetType[i].NextKeys[type] < track->KeyCounts[type] && \
         CurrentAnimation                                         \
                 ->dataType[track->KeyOffsets[type] +             \
                            targetType[i].NextKeys[type]]         \
                 .Time <= CurrentTime) {                          \
    targetType[i].CurrentKeys[type]++;                            \
    targetType[i].NextKeys[type]++;                               \
  }                                                               \
  (void)0

#define TrackInterpolate(targetType, dataType, type, dest, func)       \
  if (track->KeyCounts[type]) {                                        \
    if (targetType[i].NextKeys[type] < track->KeyCounts[type]) {       \
      float nextTime = CurrentAnimation                                \
                           ->dataType[track->KeyOffsets[type] +        \
                                      targetType[i].NextKeys[type]]    \
                           .Time;                                      \
      float prevTime = CurrentAnimation                                \
                           ->dataType[track->KeyOffsets[type] +        \
                                      targetType[i].CurrentKeys[type]] \
                           .Time;                                      \
      float duration = nextTime - prevTime;                            \
      float timeAlongDuration = CurrentTime - prevTime;                \
      float factor = timeAlongDuration / duration;                     \
      dest = glm::func(CurrentAnimation                                \
                           ->dataType[track->KeyOffsets[type] +        \
                                      targetType[i].CurrentKeys[type]] \
                           .Value,                                     \
                       CurrentAnimation                                \
                           ->dataType[track->KeyOffsets[type] +        \
                                      targetType[i].NextKeys[type]]    \
                           .Value,                                     \
                       factor);                                        \
    } else {                                                           \
      dest = CurrentAnimation                                          \
                 ->dataType[track->KeyOffsets[type] +                  \
                            targetType[i].CurrentKeys[type]]           \
                 .Value;                                               \
    }                                                                  \
  }                                                                    \
  void(0)

#define TrackNoInterpolate(targetType, dataType, type, dest) \
  if (track->KeyCounts[type]) {                              \
    dest = CurrentAnimation                                  \
               ->dataType[track->KeyOffsets[type] +          \
                          targetType[i].CurrentKeys[type]]   \
               .Value;                                       \
  }                                                          \
  void(0)

void ModelAnimator::Update(float dt) {
  if (!IsPlaying || CurrentAnimation == 0 || Character == 0) return;

  CurrentTime += dt;
  if (CurrentTime > LoopEnd) {
    if (OneShot) {
      IsPlaying = false;
      return;
    }
    float remainder = CurrentTime - LoopEnd;
    Reset();
    CurrentTime = LoopStart;
    CurrentTime += remainder;
  }

  for (int i = 0; i < CurrentAnimation->BoneTrackCount; i++) {
    BoneTrack* track = &CurrentAnimation->BoneTracks[i];
    Transform* transform = &Character->CurrentPose[track->Bone].LocalTransform;

    TrackIncrementLoop(BoneKeys, CoordKeyframes, BKT_TranslateX);
    TrackIncrementLoop(BoneKeys, CoordKeyframes, BKT_TranslateY);
    TrackIncrementLoop(BoneKeys, CoordKeyframes, BKT_TranslateZ);
    TrackIncrementLoop(BoneKeys, CoordKeyframes, BKT_ScaleX);
    TrackIncrementLoop(BoneKeys, CoordKeyframes, BKT_ScaleY);
    TrackIncrementLoop(BoneKeys, CoordKeyframes, BKT_ScaleZ);
    TrackIncrementLoop(BoneKeys, QuatKeyframes, BKT_Rotate);

    if (Tweening) {
      TrackInterpolate(BoneKeys, CoordKeyframes, BKT_TranslateX,
                       transform->Position.x, mix);
      TrackInterpolate(BoneKeys, CoordKeyframes, BKT_TranslateY,
                       transform->Position.y, mix);
      TrackInterpolate(BoneKeys, CoordKeyframes, BKT_TranslateZ,
                       transform->Position.z, mix);
      TrackInterpolate(BoneKeys, CoordKeyframes, BKT_ScaleX, transform->Scale.x,
                       mix);
      TrackInterpolate(BoneKeys, CoordKeyframes, BKT_ScaleY, transform->Scale.y,
                       mix);
      TrackInterpolate(BoneKeys, CoordKeyframes, BKT_ScaleZ, transform->Scale.z,
                       mix);
      TrackInterpolate(BoneKeys, QuatKeyframes, BKT_Rotate, transform->Rotation,
                       slerp);
    } else {
      TrackNoInterpolate(BoneKeys, CoordKeyframes, BKT_TranslateX,
                         transform->Position.x);
      TrackNoInterpolate(BoneKeys, CoordKeyframes, BKT_TranslateY,
                         transform->Position.y);
      TrackNoInterpolate(BoneKeys, CoordKeyframes, BKT_TranslateZ,
                         transform->Position.z);
      TrackNoInterpolate(BoneKeys, CoordKeyframes, BKT_ScaleX,
                         transform->Scale.x);
      TrackNoInterpolate(BoneKeys, CoordKeyframes, BKT_ScaleY,
                         transform->Scale.y);
      TrackNoInterpolate(BoneKeys, CoordKeyframes, BKT_ScaleZ,
                         transform->Scale.z);
      TrackNoInterpolate(BoneKeys, QuatKeyframes, BKT_Rotate,
                         transform->Rotation);
    }
  }

  for (int i = 0; i < CurrentAnimation->MeshTrackCount; i++) {
    MeshTrack* track = &CurrentAnimation->MeshTracks[i];
    AnimatedMesh* animatedMesh = &Character->MeshAnimStatus[track->Mesh];

    TrackIncrementLoop(MeshKeys, CoordKeyframes, MKT_Visible);
    for (int j = MKT_MorphInfluenceStart;
         j < track->MorphTargetCount + MKT_MorphInfluenceStart; j++) {
      TrackIncrementLoop(MeshKeys, CoordKeyframes, j);
    }

    TrackNoInterpolate(MeshKeys, CoordKeyframes, MKT_Visible,
                       animatedMesh->Visible);

    if (Tweening) {
      for (int j = MKT_MorphInfluenceStart;
           j < (track->MorphTargetCount + MKT_MorphInfluenceStart); j++) {
        TrackInterpolate(
            MeshKeys, CoordKeyframes, j,
            animatedMesh->MorphInfluences
                [track->MorphTargetIds[j - MKT_MorphInfluenceStart]],
            mix);
      }
    } else {
      for (int j = MKT_MorphInfluenceStart;
           j < (track->MorphTargetCount + MKT_MorphInfluenceStart); j++) {
        TrackNoInterpolate(
            MeshKeys, CoordKeyframes, j,
            animatedMesh->MorphInfluences
                [track->MorphTargetIds[j - MKT_MorphInfluenceStart]]);
      }
    }
  }
}

void ModelAnimator::Seek(float t) {
  assert(Character != 0);
  assert(CurrentAnimation != 0);
  assert(t <= CurrentAnimation->Duration);

  bool isPlaying = IsPlaying;
  float loopStart = LoopStart;
  float loopEnd = LoopEnd;

  IsPlaying = true;
  LoopStart = 0.0f;
  LoopEnd = CurrentAnimation->Duration;

  Reset();
  CurrentTime = 0.0f;
  Update(t);

  IsPlaying = isPlaying;
  LoopStart = loopStart;
  LoopEnd = loopEnd;
}

}  // namespace Impacto