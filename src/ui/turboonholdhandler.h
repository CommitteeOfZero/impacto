#pragma once

namespace Impacto {
namespace UI {

class TurboOnHoldHandler {
 public:
  TurboOnHoldHandler(float holdTime, float fireInterval, int buttonMask);

  void Update(float dt);
  void Reset();

  int ShouldFire() const;
  int Held() const;

  int PADinputButtonHoldMask;

 private:
  const float HoldTime;
  const float FireInterval;

  float CurrentHeldTime = 0.0f;

  bool EventFired = false;
};

}  // namespace UI
}  // namespace Impacto
