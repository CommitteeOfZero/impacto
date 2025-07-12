#include "turboonholdhandler.h"

#include "../vm/interface/input.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Vm::Interface;

TurboOnHoldHandler::TurboOnHoldHandler(float holdTime, float fireInterval,
                                       int buttonMask)
    : HoldTime(holdTime),
      FireInterval(fireInterval),
      PADinputButtonHoldMask(buttonMask) {}

void TurboOnHoldHandler::Update(float dt) {
  if (!(PADinputButtonIsDown & PADinputButtonHoldMask)) {
    CurrentHeldTime = 0.0f;
    EventFired = false;
    IsTurbo = false;
    return;
  }

  CurrentHeldTime += dt;

  const bool justSurpassedHoldTime =
      CurrentHeldTime >= HoldTime && CurrentHeldTime - dt < HoldTime;
  const bool surpassedFireInterval = CurrentHeldTime >= HoldTime + FireInterval;
  if (!EventFired && (justSurpassedHoldTime || surpassedFireInterval)) {
    EventFired = true;
    IsTurbo = true;
    CurrentHeldTime = HoldTime;

    return;
  }

  EventFired = false;
}

void TurboOnHoldHandler::Reset() {
  CurrentHeldTime = 0.0f;
  EventFired = false;
}

int TurboOnHoldHandler::ShouldFire() const {
  return PADinputButtonHoldMask &
         (EventFired ? PADinputButtonIsDown : PADinputButtonWentDown);
}

int TurboOnHoldHandler::Held() const {
  return PADinputButtonIsDown & PADinputButtonHoldMask;
}

}  // namespace UI
}  // namespace Impacto
