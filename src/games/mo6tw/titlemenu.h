#pragma once

#include "../../hud/titlemenu.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::TitleMenu;

class TitleMenu : public TitleMenuBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void DrawTitleMenuBackGraphics();
  void DrawTitleMenuItems();
  void DrawTitleMenuItemsLoad();
  void DrawTitleMenuItemsExtras();
  void DrawTitleMenuItemsSystem();

  Animation ItemsFadeInAnimation;
  Animation SecondaryItemsFadeInAnimation;
};

}  // namespace MO6TW
}  // namespace Impacto