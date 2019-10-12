#pragma once

#include "../../hud/sysmesbox.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::SysMesBox;

class SysMesBox : public SysMesBoxBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();
};

}  // namespace CHLCC
}  // namespace Impacto