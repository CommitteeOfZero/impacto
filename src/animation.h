#pragma once

#include "mem.h"
#include "profile/scriptvars.h"

namespace Impacto {

enum AnimationState { AS_Stopped, AS_Playing };
enum AnimationLoopMode { ALM_Stop, ALM_ReverseDirection, ALM_Loop };

class Animation {
 public:
  float DurationIn = 0;
  float DurationOut = 0;
  // 1 = in, -1 = out
  float Direction = 1;
  // 0 = fully out, 1 = fully in
  float Progress = 0;
  AnimationState State = AS_Stopped;
  AnimationLoopMode LoopMode = ALM_Stop;
  // Animation skips to the end when skip mode is enabled
  bool SkipOnSkipMode = true;

  void SetDuration(float duration) {
    DurationIn = duration;
    DurationOut = duration;
  }

  void StartIn(bool reset = false) {
    if (reset) Progress = 0;
    Direction = 1;
    State = AS_Playing;
    StartInImpl(reset);
  }
  void StartOut(bool reset = false) {
    if (reset) Progress = 1;
    Direction = -1;
    State = AS_Playing;
    StartOutImpl(reset);
  }
  void Update(float dt) {
    if (State == AS_Stopped) return;
    UpdateImpl(dt);
  }

  virtual void Render() {}

  bool IsOut() const { return Progress == 0 && State == AS_Stopped; }
  bool IsIn() const { return Progress == 1 && State == AS_Stopped; }

 protected:
  virtual void StartInImpl(bool reset = false) {}
  virtual void StartOutImpl(bool reset = false) {}
  virtual void UpdateImpl(float dt) {
    if (SkipOnSkipMode && GetFlag(Profile::ScriptVars::SF_MESALLSKIP) &&
        State != AS_Stopped) {
      Progress = Direction == 1 ? 1.0f : 0.0f;
    }
    AddDelta(dt);
  }

  void AddDelta(float dt) {
    float duration = Direction == 1 ? DurationIn : DurationOut;

    switch (LoopMode) {
      case ALM_Stop: {
        float endProgress = Direction == 1 ? 1.0f : 0.0f;

        Progress = std::clamp(Progress + Direction * dt / duration, 0.0f, 1.0f);
        if (Progress == endProgress) State = AS_Stopped;
        return;
      }
      case ALM_Loop: {
        // E.g. Progress = 1.04 => Progress = 0.04
        Progress += Direction * dt / duration;
        Progress -= std::floor(Progress);
        return;
      }
      case ALM_ReverseDirection: {
        // E.g. Progress = 1.04 => Progress = 0.96
        float cycleDuration = DurationIn + DurationOut;

        float time = Progress * duration;
        if (Direction == -1) time = cycleDuration - time;

        time = std::fmod(time + dt, cycleDuration);

        if (time < DurationIn) {
          Progress = time / DurationIn;
          Direction = 1;
        } else {
          Progress = 1.0f - (time - DurationIn) / DurationOut;
          Direction = -1;
        }

        return;
      }
    }
  }
};

}  // namespace Impacto