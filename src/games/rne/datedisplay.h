#pragma once

#include "../../hud/datedisplay.h"

namespace Impacto {
namespace DateDisplay {

class RNEDateDisplay : public DateDisplay {
 public:
  void Update(float dt);
  void Render();
};

}  // namespace DateDisplay
}  // namespace Impacto