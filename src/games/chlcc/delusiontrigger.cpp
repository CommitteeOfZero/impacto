#include "delusiontrigger.h"
#include "../../renderer/renderer.h"
#include "../../vm/interface/input.h"
#include "../../mem.h"
#include "../../profile/hud/delusiontrigger.h"
#include "../../profile/games/chlcc/delusiontrigger.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::DelusionTrigger;
using namespace Impacto::Profile::DelusionTrigger;
using namespace Impacto::Vm::Interface;

DelusionTrigger::DelusionTrigger()
    : DelusionTriggerBase(ScrWork[3430], Hidden) {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  TextFade.Direction = 1.0f;
  TextFade.LoopMode = ALM_Stop;
  TextFade.DurationIn = TextFadeInDuration;
  TextFade.DurationOut = TextFadeOutDuration;
}

void DelusionTrigger::Reset() {
  ImpLogSlow(LL_Warning, LC_VMStub, "Resetting Delusion Trigger\n");
  DelusionState = DELUSION_STATE::NEUTRAL;
}

void DelusionTrigger::Show() {
  if (State != Shown && State != Showing) {
    State = Showing;
    DelusionState = DELUSION_STATE::NEUTRAL;
    maskScaleFactor = 0.1f;
    spinAngle = 0.0f;
    spinRate = 0.5f;
    spinDirection = 1;
    underLayerAlpha = 0;
    FadeAnimation.StartIn();
    SetFlag(2511, 1);
  }
}
void DelusionTrigger::Hide() {
  if (State != Hidden && State != Hiding) {
    State = Hiding;
    SetFlag(2511, 1);
    FadeAnimation.StartOut();
  }
}

void DelusionTrigger::Update(float dt) {
  if (State == Showing) {
    if (FadeAnimation.IsIn()) {
      State = Shown;
      SetFlag(2511, 0);
    } else {
      underLayerAlpha =
          FadeAnimation.Progress < 0.5f ? FadeAnimation.Progress : 0.5f;
      if (FadeAnimation.Progress < 148 / 60 / FadeAnimation.DurationIn) {
        maskScaleFactor += dt;
      } else if (FadeAnimation.Progress >
                    ((148 + 25) / 60.0f) / FadeAnimation.DurationIn &&
                FadeAnimation.Progress <
                    ((148 + 25 + 50) / 60.0f) / FadeAnimation.DurationIn) {
        maskScaleFactor -= 1.75f * dt;

      }
    }
  } else if (State == Hiding) {
    if (FadeAnimation.IsOut()) {
      State = Hidden;
      SetFlag(2511, 0);
      ImpLogSlow(LL_Warning, LC_VMStub, "Locking in delusion trigger %d\n",
                 DelusionState);
    } else {
      underLayerAlpha =
          FadeAnimation.Progress < 0.5f ? FadeAnimation.Progress : 0.5f;
      if (FadeAnimation.Progress > (1 - 100 / 60.0f / FadeAnimation.DurationIn)) {
        maskScaleFactor += dt;
      } else if (FadeAnimation.Progress <
                    ( 1 - ((100 + 25) / 60.0f) / FadeAnimation.DurationIn) && maskScaleFactor > 0) {
        maskScaleFactor -= 2.5f * dt;
      }
    }
  } else if (State == Shown) {
    if (PADinputButtonWentDown & PAD1L2) {
      switch (ScrWork[3430]) {
        case DELUSION_STATE::NEUTRAL:
          ScrWork[3430] = DELUSION_STATE::POSITIVE;
          spinRate = 1.5f;
          spinDirection = 1;
          underLayerAlpha = 0.875f;
          break;
        case DELUSION_STATE::NEGATIVE:
          ScrWork[3430] = DELUSION_STATE::NEUTRAL;
          spinRate = 0.5f;
          underLayerAlpha = 0.5f;
          break;
        case DELUSION_STATE::POSITIVE:
        default:
          break;
      }
    } else if (PADinputButtonWentDown & PAD1R2) {
      switch (ScrWork[3430]) {
        case DELUSION_STATE::NEUTRAL:
          ScrWork[3430] = DELUSION_STATE::NEGATIVE;
          spinRate = 1.5f;
          spinDirection = -1;
          underLayerAlpha = 0.875f;
          break;
        case DELUSION_STATE::POSITIVE:
          ScrWork[3430] = DELUSION_STATE::NEUTRAL;
          spinRate = 0.5f;
          underLayerAlpha = 0.5f;
          break;
        case DELUSION_STATE::NEGATIVE:
        default:
          break;
      }
    }
    ImpLogSlow(LL_Warning, LC_VMStub, "Delusion Trigger Set To %d\n",
               ScrWork[3430]);
  }

  if (State != Hidden) {
    FadeAnimation.Update(dt);
    spinAngle -= spinDirection * spinRate * dt;
    if (spinAngle > 2.0f * M_PI)
      spinAngle -= 2.0f * M_PI;
    else if (spinAngle < 0.0f)
      spinAngle += 2.0f * M_PI;
  }
}

void DelusionTrigger::Render() {
  if (State == Hidden) return;
  Sprite ScaledMask = BackgroundSpriteMask;
  // float scalingFactor = 1;
  constexpr float aspect_ratio = 1280.0f / 720.0f;

  ImpLogSlow(LL_Warning, LC_VMStub, "Scaling factor is %f\n", maskScaleFactor);

  float newWidth =
      BackgroundSpriteMask.Bounds.Width * maskScaleFactor * aspect_ratio;
  float newHeight = BackgroundSpriteMask.Bounds.Height * maskScaleFactor;

  float deltaWidth = newWidth - BackgroundSpriteMask.Bounds.Width;
  float deltaHeight = newHeight - BackgroundSpriteMask.Bounds.Height;

  ScaledMask.Bounds.Width = newWidth;
  ScaledMask.Bounds.Height = newHeight;

  ScaledMask.Bounds.X = BackgroundSpriteMask.Bounds.X - deltaWidth / 2.0f;
  ScaledMask.Bounds.Y = BackgroundSpriteMask.Bounds.Y - deltaHeight / 2.0f;

  Renderer->DrawSprite(ScreenMask,
                             RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                             glm::vec4(1.0f, 1.0f, 1.0f, underLayerAlpha));

  Renderer->DrawCHLCCDelusionOverlay(BackgroundSprite, ScaledMask,
                                     RectF(0.0f, 0.0f, 1280.0f, 720.0f), 155,
                                     32, spinAngle);
}

}  // namespace CHLCC
}  // namespace Impacto