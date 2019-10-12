#pragma once

#include "../../hud/sysmesbox.h"

namespace Impacto {
namespace RNE {

using namespace Impacto::SysMesBox;

class SysMesBox : public SysMesBoxBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();
};

}  // namespace RNE
}  // namespace Impacto