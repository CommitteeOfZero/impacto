#pragma once

#include "mem.h"
#include "profile/scriptvars.h"
#include <enum.h>

namespace Impacto {

enum class AnimationState { Stopped = 0, Playing = 1 };
enum class AnimationLoopMode { Stop = 0, ReverseDirection = 1, Loop = 2 };

enum class AnimationDirection { In = 1, Out = -1 };
constexpr AnimationDirection operator-(AnimationDirection direction) {
  return static_cast<AnimationDirection>(static_cast<int>(direction) * -1);
}

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
  bool SkipOnSkipMode = false;

  void SetDuration(float duration) {
    DurationIn = duration;
    DurationOut = duration;
  }
  float GetDuration(AnimationDirection direction) const {
    return direction == AnimationDirection::In ? DurationIn : DurationOut;
  }

  void Stop() { State = AnimationState::Stopped; }

  void StartIn(bool reset = false) {
    if (reset) Progress = 0.0f;
    Direction = AnimationDirection::In;
    State = AnimationState::Playing;
    StartInImpl(reset);
  }

  void StartOut(bool reset = false) {
    if (reset) Progress = 1.0f;
    Direction = AnimationDirection::Out;
    State = AnimationState::Playing;
    StartOutImpl(reset);
  }

  void Start(AnimationDirection direction, bool reset = false) {
    if (direction == AnimationDirection::In)
      StartIn(reset);
    else
      StartOut(reset);
  }

  void Finish() {
    State = AnimationState::Stopped;
    Progress = Direction == AnimationDirection::In ? 1.0f : 0.0f;
    FinishImpl();
  }

  void Update(float dt) {
    if (State == AnimationState::Stopped) return;
    UpdateImpl(dt);
  }

  virtual void Render() {}

  bool IsOut() const {
    return Progress == 0.0f && State == AnimationState::Stopped;
  }
  bool IsIn() const {
    return Progress == 1.0f && State == AnimationState::Stopped;
  }
  bool IsFinished(AnimationDirection direction) const {
    return direction == AnimationDirection::In ? IsIn() : IsOut();
  }

  bool IsPlaying() const { return State == AnimationState::Playing; }
  bool IsStopped() const { return State == AnimationState::Stopped; }

 protected:
  void AddDelta(float dt);
  virtual void StartInImpl(bool reset = false) {}
  virtual void StartOutImpl(bool reset = false) {}
  virtual void FinishImpl() {};
  virtual void UpdateImpl(float dt) {
    if (SkipOnSkipMode && GetFlag(Profile::ScriptVars::SF_MESALLSKIP) &&
        State != AnimationState::Stopped) {
      Progress = Direction == AnimationDirection::In ? 1.0f : 0.0f;
    }
    AddDelta(dt);
  }
};

}  // namespace Impacto