#pragma once

#include <enum.h>

namespace Impacto {
namespace DateDisplay {

enum DateAnimState { Hidden, Hiding, Showing, Shown };

BETTER_ENUM(DateDisplayType, int, None, RNE, Darling)

class DateDisplay {
 public:
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  DateAnimState AnimState = DateAnimState::Hidden;

 protected:
  float Fade = 0.0f;

  int Day = 0;
  int Month = 0;
  int Year = 0;
  int Week = 0;
};

extern DateDisplay* Implementation;

void Update(float dt);
void Render();

}  // namespace DateDisplay
}  // namespace Impacto