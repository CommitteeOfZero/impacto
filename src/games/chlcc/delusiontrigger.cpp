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

  TitleFade.Direction = 1.0f;
  TitleFade.LoopMode = ALM_Stop;
  TitleFade.DurationIn = TextFadeInDuration;
  TitleFade.DurationOut = TextFadeOutDuration;
}

void DelusionTrigger::Reset() {
  ImpLogSlow(LL_Warning, LC_VMStub, "Resetting Delusion Trigger\n");
  DelusionState = DELUSION_STATE::NEUTRAL;
}

void DelusionTrigger::Show() {
  if (State != Shown && State != Showing) {
    State = Showing;
    DelusionState = DELUSION_STATE::NEUTRAL;
    SetFlag(2511, 1);
    FadeAnimation.StartIn();
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
  if (State == Showing && FadeAnimation.IsIn()) {
    State = Shown;
    SetFlag(2511, 0);
  } else if (State == Hiding && FadeAnimation.IsOut()) {
    State = Hidden;
    SetFlag(2511, 0);
    ImpLogSlow(LL_Warning, LC_VMStub, "Locking in delusion trigger %d\n",
               DelusionState);
  } else if (State == Shown) {
    if (PADinputButtonWentDown & PAD1L2) {
      switch (ScrWork[3430]) {
        case DELUSION_STATE::NEUTRAL:
          ScrWork[3430] = DELUSION_STATE::POSITIVE;
          break;
        case DELUSION_STATE::NEGATIVE:
          ScrWork[3430] = DELUSION_STATE::NEUTRAL;
          break;
        case DELUSION_STATE::POSITIVE:
        default:
          break;
      }
    } else if (PADinputButtonWentDown & PAD1R2) {
      switch (ScrWork[3430]) {
        case DELUSION_STATE::NEUTRAL:
          ScrWork[3430] = DELUSION_STATE::NEGATIVE;
          break;
        case DELUSION_STATE::POSITIVE:
          ScrWork[3430] = DELUSION_STATE::NEUTRAL;
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
  }
}

void DelusionTrigger::Render() {
  if (State == Hidden) return;

  Renderer->DrawSprite(ScreenMask,
                             RectF(0.0f, 0.0f, 1280.0f, 720.0f));
  Renderer->DrawMaskedSprite(BackgroundSprite, BackgroundSpriteMask,
                             RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                             glm::vec4(1.0f), 155, 32, false, true);
}

}  // namespace CHLCC
}  // namespace Impacto