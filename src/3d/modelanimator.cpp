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

  CurrentTime = 0.0f;

  for (int i = 0; i < CurrentAnimation->BoneTrackCount; i++) {
    BoneKeys[i].CurrentTranslateXKey = 0;
    BoneKeys[i].CurrentTranslateYKey = 0;
    BoneKeys[i].CurrentTranslateZKey = 0;
    BoneKeys[i].CurrentScaleXKey = 0;
    BoneKeys[i].CurrentScaleYKey = 0;
    BoneKeys[i].CurrentScaleZKey = 0;
    BoneKeys[i].CurrentRotateKey = 0;

    BoneKeys[i].NextTranslateXKey = 1;
    BoneKeys[i].NextTranslateYKey = 1;
    BoneKeys[i].NextTranslateZKey = 1;
    BoneKeys[i].NextScaleXKey = 1;
    BoneKeys[i].NextScaleYKey = 1;
    BoneKeys[i].NextScaleZKey = 1;
    BoneKeys[i].NextRotateKey = 1;
  }
}

// For each sub-track, if we're not on the last keyframe already, make sure the
// *next* keyframe is *after* the current time
#define BoneTrackIncrementLoop(dataType, type)                        \
  while (BoneKeys[i].Next##type##Key < track->##type##Count &&        \
         CurrentAnimation                                             \
                 ->##dataType##Keyframes[track->##type##Offset +      \
                                         BoneKeys[i].Next##type##Key] \
                 .Time <= CurrentTime) {                              \
    BoneKeys[i].Current##type##Key++;                                 \
    BoneKeys[i].Next##type##Key++;                                    \
  }                                                                   \
  (void)0

#define BoneTrackInterpolate(dataType, type, dest, func)                 \
  if (track->##type##Count) {                                            \
    if (BoneKeys[i].Next##type##Key < track->##type##Count) {            \
      float nextTime =                                                   \
          CurrentAnimation                                               \
              ->##dataType##Keyframes[track->##type##Offset +            \
                                      BoneKeys[i].Next##type##Key]       \
              .Time;                                                     \
      float prevTime =                                                   \
          CurrentAnimation                                               \
              ->##dataType##Keyframes[track->##type##Offset +            \
                                      BoneKeys[i].Current##type##Key]    \
              .Time;                                                     \
      float duration = nextTime - prevTime;                              \
      float timeAlongDuration = CurrentTime - prevTime;                  \
      float factor = timeAlongDuration / duration;                       \
      dest = glm::##func(                                                \
          CurrentAnimation                                               \
              ->##dataType##Keyframes[track->##type##Offset +            \
                                      BoneKeys[i].Current##type##Key]    \
              .Value,                                                    \
          CurrentAnimation                                               \
              ->##dataType##Keyframes[track->##type##Offset +            \
                                      BoneKeys[i].Next##type##Key]       \
              .Value,                                                    \
          factor);                                                       \
    } else {                                                             \
      dest = CurrentAnimation                                            \
                 ->##dataType##Keyframes[track->##type##Offset +         \
                                         BoneKeys[i].Current##type##Key] \
                 .Value;                                                 \
    }                                                                    \
  }                                                                      \
  void(0)

#define BoneTrackNoInterpolate(dataType, type, dest)                   \
  if (track->##type##Count) {                                          \
    dest = CurrentAnimation                                            \
               ->##dataType##Keyframes[track->##type##Offset +         \
                                       BoneKeys[i].Current##type##Key] \
               .Value;                                                 \
  }                                                                    \
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

    BoneTrackIncrementLoop(Coord, TranslateX);
    BoneTrackIncrementLoop(Coord, TranslateY);
    BoneTrackIncrementLoop(Coord, TranslateZ);
    BoneTrackIncrementLoop(Coord, ScaleX);
    BoneTrackIncrementLoop(Coord, ScaleY);
    BoneTrackIncrementLoop(Coord, ScaleZ);
    BoneTrackIncrementLoop(Quat, Rotate);

    if (Tweening) {
      BoneTrackInterpolate(Coord, TranslateX, transform->Position.x, mix);
      BoneTrackInterpolate(Coord, TranslateY, transform->Position.y, mix);
      BoneTrackInterpolate(Coord, TranslateZ, transform->Position.z, mix);
      BoneTrackInterpolate(Coord, ScaleX, transform->Scale.x, mix);
      BoneTrackInterpolate(Coord, ScaleY, transform->Scale.y, mix);
      BoneTrackInterpolate(Coord, ScaleZ, transform->Scale.z, mix);
      BoneTrackInterpolate(Quat, Rotate, transform->Rotation, slerp);
    } else {
      BoneTrackNoInterpolate(Coord, TranslateX, transform->Position.x);
      BoneTrackNoInterpolate(Coord, TranslateY, transform->Position.y);
      BoneTrackNoInterpolate(Coord, TranslateZ, transform->Position.z);
      BoneTrackNoInterpolate(Coord, ScaleX, transform->Scale.x);
      BoneTrackNoInterpolate(Coord, ScaleY, transform->Scale.y);
      BoneTrackNoInterpolate(Coord, ScaleZ, transform->Scale.z);
      BoneTrackNoInterpolate(Quat, Rotate, transform->Rotation);
    }
  }
}

}  // namespace Impacto