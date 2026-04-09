#pragma once

#include <magic_enum/magic_enum.hpp>
#include "../animation.h"

namespace Impacto {
namespace DateDisplay {

enum class DateDisplayType : int {
  None,
  RNE,
  Darling,
};
class DateDisplayBase {
 public:
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  Animation FadeAnimation;

 protected:
  int Day = 0;
  int Month = 0;
  int Year = 0;
  int Week = 0;
};

inline DateDisplayBase* Implementation = nullptr;

void Init();
void Update(float dt);
void Render();

}  // namespace DateDisplay
}  // namespace Impacto