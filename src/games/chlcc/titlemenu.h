#pragma once

#include "../../hud/titlemenu.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::TitleMenu;

class TitleMenu : public TitleMenuBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();
};

}  // namespace CHLCC
}  // namespace Impacto