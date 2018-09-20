#include "modelanimator.h"

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

#define TrackIncrementLoop(dataType, type)                            \
  while (BoneKeys[i].Next##type##Key < track->##type##Count &&        \
         CurrentAnimation                                             \
                 ->##dataType##Keyframes[track->##type##Offset +      \
                                         BoneKeys[i].Next##type##Key] \
                 .Time <= CurrentTime) {                              \
    BoneKeys[i].Current##type##Key++;                                 \
    BoneKeys[i].Next##type##Key++;                                    \
  }                                                                   \
  (void)0

#define TrackInterpolate(dataType, type, dest, func)                     \
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

#define TrackNoInterpolate(dataType, type, dest)                       \
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

    TrackIncrementLoop(Coord, TranslateX);
    TrackIncrementLoop(Coord, TranslateY);
    TrackIncrementLoop(Coord, TranslateZ);
    TrackIncrementLoop(Coord, ScaleX);
    TrackIncrementLoop(Coord, ScaleY);
    TrackIncrementLoop(Coord, ScaleZ);
    TrackIncrementLoop(Quat, Rotate);

    if (Tweening) {
      TrackInterpolate(Coord, TranslateX, transform->Position.x, mix);
      TrackInterpolate(Coord, TranslateY, transform->Position.y, mix);
      TrackInterpolate(Coord, TranslateZ, transform->Position.z, mix);
      TrackInterpolate(Coord, ScaleX, transform->Scale.x, mix);
      TrackInterpolate(Coord, ScaleY, transform->Scale.y, mix);
      TrackInterpolate(Coord, ScaleZ, transform->Scale.z, mix);
      TrackInterpolate(Quat, Rotate, transform->Rotation, slerp);
    } else {
      TrackNoInterpolate(Coord, TranslateX, transform->Position.x);
      TrackNoInterpolate(Coord, TranslateY, transform->Position.y);
      TrackNoInterpolate(Coord, TranslateZ, transform->Position.z);
      TrackNoInterpolate(Coord, ScaleX, transform->Scale.x);
      TrackNoInterpolate(Coord, ScaleY, transform->Scale.y);
      TrackNoInterpolate(Coord, ScaleZ, transform->Scale.z);
      TrackNoInterpolate(Quat, Rotate, transform->Rotation);
    }
  }
}

}  // namespace Impacto