#pragma once

#include "../datedisplay.h"

namespace Impacto {
namespace RNE {

using namespace Impacto::DateDisplay;

class DateDisplay : public DateDisplayBase {
 public:
  void Update(float dt);
  void Render();
};

}  // namespace RNE
}  // namespace Impacto