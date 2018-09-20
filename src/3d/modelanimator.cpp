#include "modelanimator.h"

#include "character3d.h"

namespace Impacto {

void ModelAnimator::Start(uint16_t animId) {
  assert(Character != 0);

  CurrentAnimation = Character->StaticModel->Animations.at(animId);

  Reset();
}
void ModelAnimator::Reset() {
  assert(Character != 0);
  assert(CurrentAnimation != 0);

  Character->ReloadDefaultBoneTransforms();
  Character->ReloadDefaultMeshAnimStatus();

  CurrentTime = 0.0f;

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

    Character->MeshAnimStatus[CurrentAnimation->MeshTracks[i].Mesh]
        .UsedMorphTargetCount =
        CurrentAnimation->MeshTracks[i].MorphTargetCount;
    for (int j = 0; j < CurrentAnimation->MeshTracks[i].MorphTargetCount; j++) {
      Character->MeshAnimStatus[CurrentAnimation->MeshTracks[i].Mesh]
          .UsedMorphTargetIds[j] =
          CurrentAnimation->MeshTracks[i].MorphTargetIds[j];
    }
  }
}

// For each sub-track, if we're not on the last keyframe already, make sure the
// *next* keyframe is *after* the current time
#define TrackIncrementLoop(targetType, dataType, type)                         \
  while (##targetType##Keys[i].NextKeys[type] < track->KeyCounts[type] &&      \
         CurrentAnimation                                                      \
                 ->##dataType##Keyframes[track->KeyOffsets[type] +             \
                                         ##targetType##Keys[i].NextKeys[type]] \
                 .Time <= CurrentTime) {                                       \
    ##targetType##Keys[i].CurrentKeys[type]++;                                 \
    ##targetType##Keys[i].NextKeys[type]++;                                    \
  }                                                                            \
  (void)0

#define TrackInterpolate(targetType, dataType, type, dest, func)               \
  if (track->KeyCounts[type]) {                                                \
    if (##targetType##Keys[i].NextKeys[type] < track->KeyCounts[type]) {       \
      float nextTime =                                                         \
          CurrentAnimation                                                     \
              ->##dataType##Keyframes[track->KeyOffsets[type] +                \
                                      ##targetType##Keys[i].NextKeys[type]]    \
              .Time;                                                           \
      float prevTime =                                                         \
          CurrentAnimation                                                     \
              ->##dataType##Keyframes[track->KeyOffsets[type] +                \
                                      ##targetType##Keys[i].CurrentKeys[type]] \
              .Time;                                                           \
      float duration = nextTime - prevTime;                                    \
      float timeAlongDuration = CurrentTime - prevTime;                        \
      float factor = timeAlongDuration / duration;                             \
      dest = glm::##func(                                                      \
          CurrentAnimation                                                     \
              ->##dataType##Keyframes[track->KeyOffsets[type] +                \
                                      ##targetType##Keys[i].CurrentKeys[type]] \
              .Value,                                                          \
          CurrentAnimation                                                     \
              ->##dataType##Keyframes[track->KeyOffsets[type] +                \
                                      ##targetType##Keys[i].NextKeys[type]]    \
              .Value,                                                          \
          factor);                                                             \
    } else {                                                                   \
      dest =                                                                   \
          CurrentAnimation                                                     \
              ->##dataType##Keyframes[track->KeyOffsets[type] +                \
                                      ##targetType##Keys[i].CurrentKeys[type]] \
              .Value;                                                          \
    }                                                                          \
  }                                                                            \
  void(0)

#define TrackNoInterpolate(targetType, dataType, type, dest)                 \
  if (track->KeyCounts[type]) {                                              \
    dest =                                                                   \
        CurrentAnimation                                                     \
            ->##dataType##Keyframes[track->KeyOffsets[type] +                \
                                    ##targetType##Keys[i].CurrentKeys[type]] \
            .Value;                                                          \
  }                                                                          \
  void(0)

void ModelAnimator::Update(float dt) {
  assert(Character != 0);
  assert(CurrentAnimation != 0);

  CurrentTime += dt;
  if (CurrentTime > CurrentAnimation->Duration) {
    float remainder = CurrentTime - CurrentAnimation->Duration;
    Reset();
    CurrentTime += remainder;
  }

  for (int i = 0; i < CurrentAnimation->BoneTrackCount; i++) {
    BoneTrack* track = &CurrentAnimation->BoneTracks[i];
    Transform* transform = &Character->CurrentPose[track->Bone].LocalTransform;

    TrackIncrementLoop(Bone, Coord, BKT_TranslateX);
    TrackIncrementLoop(Bone, Coord, BKT_TranslateY);
    TrackIncrementLoop(Bone, Coord, BKT_TranslateZ);
    TrackIncrementLoop(Bone, Coord, BKT_ScaleX);
    TrackIncrementLoop(Bone, Coord, BKT_ScaleY);
    TrackIncrementLoop(Bone, Coord, BKT_ScaleZ);
    TrackIncrementLoop(Bone, Quat, BKT_Rotate);

    if (Tweening) {
      TrackInterpolate(Bone, Coord, BKT_TranslateX, transform->Position.x, mix);
      TrackInterpolate(Bone, Coord, BKT_TranslateY, transform->Position.y, mix);
      TrackInterpolate(Bone, Coord, BKT_TranslateZ, transform->Position.z, mix);
      TrackInterpolate(Bone, Coord, BKT_ScaleX, transform->Scale.x, mix);
      TrackInterpolate(Bone, Coord, BKT_ScaleY, transform->Scale.y, mix);
      TrackInterpolate(Bone, Coord, BKT_ScaleZ, transform->Scale.z, mix);
      TrackInterpolate(Bone, Quat, BKT_Rotate, transform->Rotation, slerp);
    } else {
      TrackNoInterpolate(Bone, Coord, BKT_TranslateX, transform->Position.x);
      TrackNoInterpolate(Bone, Coord, BKT_TranslateY, transform->Position.y);
      TrackNoInterpolate(Bone, Coord, BKT_TranslateZ, transform->Position.z);
      TrackNoInterpolate(Bone, Coord, BKT_ScaleX, transform->Scale.x);
      TrackNoInterpolate(Bone, Coord, BKT_ScaleY, transform->Scale.y);
      TrackNoInterpolate(Bone, Coord, BKT_ScaleZ, transform->Scale.z);
      TrackNoInterpolate(Bone, Quat, BKT_Rotate, transform->Rotation);
    }
  }

  for (int i = 0; i < CurrentAnimation->MeshTrackCount; i++) {
    MeshTrack* track = &CurrentAnimation->MeshTracks[i];
    AnimatedMesh* animatedMesh = &Character->MeshAnimStatus[track->Mesh];

    TrackIncrementLoop(Mesh, Coord, MKT_Visible);
    for (int j = MKT_MorphInfluenceStart;
         j < track->MorphTargetCount + MKT_MorphInfluenceStart; j++) {
      TrackIncrementLoop(Mesh, Coord, j);
    }

    TrackNoInterpolate(Mesh, Coord, MKT_Visible, animatedMesh->Visible);

    if (Tweening) {
      for (int j = MKT_MorphInfluenceStart;
           j < (track->MorphTargetCount + MKT_MorphInfluenceStart); j++) {
        TrackInterpolate(
            Mesh, Coord, j,
            animatedMesh->MorphInfluences[j - MKT_MorphInfluenceStart], mix);
      }
    } else {
      for (int j = MKT_MorphInfluenceStart;
           j < (track->MorphTargetCount + MKT_MorphInfluenceStart); j++) {
        TrackNoInterpolate(
            Mesh, Coord, j,
            animatedMesh->MorphInfluences[j - MKT_MorphInfluenceStart]);
      }
    }
  }
}

}  // namespace Impacto