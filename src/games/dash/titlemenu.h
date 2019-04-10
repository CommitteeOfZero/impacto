#pragma once

#include "../../hud/titlemenu.h"

namespace Impacto {
namespace Dash {

using namespace Impacto::TitleMenu;

class TitleMenu : public TitleMenuBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  Animation* BackgroundAnimation = 0;
  Animation PreTitleItemsAnimation;
};

}  // namespace Dash
}  // namespace Impacto