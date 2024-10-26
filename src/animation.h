#pragma once

#include "mem.h"
#include "profile/scriptvars.h"
#include "enum.h"

namespace Impacto {

BETTER_ENUM(AnimationState, int, Stopped, Playing)
BETTER_ENUM(AnimationLoopMode, int, Stop, ReverseDirection, Loop)
BETTER_ENUM(AnimationDirection, int, In = 1, Out = -1)

class Animation {
 public:
  float DurationIn = 0;
  float DurationOut = 0;
  // 1 = in, -1 = out
  AnimationDirection Direction = AnimationDirection::In;
  // 0 = fully out, 1 = fully in
  float Progress = 0;
  AnimationState State = AnimationState::Stopped;
  AnimationLoopMode LoopMode = AnimationLoopMode::Stop;
  // Animation skips to the end when skip mode is enabled
  bool SkipOnSkipMode = true;

  void SetDuration(float duration) {
    DurationIn = duration;
    DurationOut = duration;
  }

  void Stop() { State = AnimationState::Stopped; }

  void StartIn(bool reset = false) {
    if (reset) Progress = 0;
    Direction = AnimationDirection::In;
    State = AnimationState::Playing;
    StartInImpl(reset);
  }

  void StartOut(bool reset = false) {
    if (reset) Progress = 1;
    Direction = AnimationDirection::Out;
    State = AnimationState::Playing;
    StartOutImpl(reset);
  }

  void Update(float dt) {
    if (State == +AnimationState::Stopped) return;
    UpdateImpl(dt);
  }

  virtual void Render() {}

  bool IsOut() const {
    return Progress == 0 && State == +AnimationState::Stopped;
  }
  bool IsIn() const {
    return Progress == 1 && State == +AnimationState::Stopped;
  }

 protected:
  void AddDelta(float dt);
  virtual void StartInImpl(bool reset = false) {}
  virtual void StartOutImpl(bool reset = false) {}
  virtual void UpdateImpl(float dt) {
    if (SkipOnSkipMode && GetFlag(Profile::ScriptVars::SF_MESALLSKIP) &&
        State != +AnimationState::Stopped) {
      Progress = Direction == +AnimationDirection::In ? 1.0f : 0.0f;
    }
    AddDelta(dt);
  }
};

}  // namespace Impacto