#include "systemmenu.h"

#include "../../profile/ui/systemmenu.h"
#include "../../profile/games/mo6tw/systemmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::Profile::MO6TW::SystemMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_SYSMENUCNO] = target->Id;
  ChoiceMade = true;
}

SystemMenu::SystemMenu() {
  MainItems = new Widgets::Group(this);

  auto onClick =
      std::bind(&SystemMenu::MenuButtonOnClick, this, std::placeholders::_1);

  for (int i = 0; i < MenuEntriesNum; i++) {
    Button* menuButton = new Button(
        i, MenuEntriesSprites[i], MenuEntriesSprites[i],
        MenuEntriesHighlightedSprite,
        glm::vec2(MenuEntriesX, MenuEntriesFirstY + (i * MenuEntriesYPadding)));

    menuButton->OnClickHandler = onClick;
    MainItems->Add(menuButton, FDIR_DOWN);
  }

  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void SystemMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void SystemMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SystemMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_SYSMENUALPHA] < 256 && State == Shown) {
    Hide();
  }

  if (ScrWork[SW_SYSMENUALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_SYSMENUALPHA] == 0 && FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}

void SystemMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSMENUALPHA] > 0) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(SystemMenuBackgroundSprite,
                           glm::vec2(SystemMenuX, SystemMenuY), col);
    MainItems->Tint = col;
    MainItems->Render();
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto