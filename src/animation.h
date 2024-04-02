#pragma once

#include "mem.h"
#include "profile/scriptvars.h"

namespace Impacto {

enum AnimationState { AS_Stopped, AS_Playing };
enum AnimationLoopMode { ALM_Stop, ALM_ReverseDirection, ALM_Loop };

class Animation {
 public:
  float DurationIn;
  float DurationOut;
  // 1 = in, -1 = out
  float Direction = 1;
  // 0 = fully out, 1 = fully in
  float Progress = 0;
  AnimationState State = AS_Stopped;
  AnimationLoopMode LoopMode = ALM_Stop;

  void StartIn(bool reset = false) {
    if (reset) Progress = 0;
    Direction = 1;
    State = AS_Playing;
    StartInImpl();
  }
  void StartOut(bool reset = false) {
    if (reset) Progress = 1;
    Direction = -1;
    State = AS_Playing;
    StartOutImpl();
  }
  void Update(float dt) {
    if (State == AS_Stopped) return;
    UpdateImpl(dt);
  }

  virtual void Render() {}

  bool IsOut() const { return Progress == 0 && State == AS_Stopped; }
  bool IsIn() const { return Progress == 1 && State == AS_Stopped; }

 protected:
  virtual void StartInImpl() {}
  virtual void StartOutImpl() {}
  virtual void UpdateImpl(float dt) {
    if (GetFlag(Profile::ScriptVars::SF_MESALLSKIP) && State != AS_Stopped) {
      Progress = Direction == 1 ? 1.0f : 0.0f;
    }
    AddDelta(dt);
  }

  void AddDelta(float dt) {
    if (Direction == 1) {
      Progress += dt / DurationIn;
      if (Progress >= 1) {
        switch (LoopMode) {
          case ALM_Stop: {
            Progress = 1;
            State = AS_Stopped;
            break;
          }
          case ALM_ReverseDirection: {
            // E.g. Progress = 1.04 => Progress = 0.96
            Progress = 1 - (Progress - 1);
            Direction = -1;
            StartOutImpl();
            break;
          }
          case ALM_Loop: {
            // E.g. Progress = 1.04 => Progress = 0.04
            Progress = (Progress - 1);
            StartInImpl();
            break;
          }
        }
      }
    } else {
      Progress -= dt / DurationOut;
      if (Progress <= 0) {
        switch (LoopMode) {
          case ALM_Stop: {
            Progress = 0;
            State = AS_Stopped;
            break;
          }
          case ALM_ReverseDirection: {
            // e.g. Progress = -0.04 => Progress = 0.04
            Progress = 0 - Progress;
            Direction = 1;
            StartInImpl();
            break;
          }
          case ALM_Loop: {
            // e.g. Progress = -0.04 => Progress = 0.96
            Progress = 1 + Progress;
            StartOutImpl();
            break;
          }
        }
      }
    }
  }
};

}  // namespace Impacto