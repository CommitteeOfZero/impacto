#include "systemmenu.h"

#include "../../profile/ui/systemmenu.h"
#include "../../profile/games/mo8/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO8 {

using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::Profile::MO8::SystemMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_SYSMENUCNO] = target->Id;
  ChoiceMade = true;
  // When exiting through a menu button remove focus from the menu to avoid
  // triggering the onClick handler again
  if (target->Id == ExitMenuButtonId)
    IsFocused = false;
  else
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 5, false, 0.0f);
}

SystemMenu::SystemMenu() {
  MainItems = new Widgets::Group(this);

  auto onClick =
      std::bind(&SystemMenu::MenuButtonOnClick, this, std::placeholders::_1);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  for (int i = 0; i < MenuEntriesNum; i++) {
    Button* menuButton = new Button(
        i, MenuEntriesSprites[i], MenuEntriesHSprites[i], nullSprite,
        glm::vec2(MenuEntriesX, MenuEntriesFirstY + (i * MenuEntriesYPadding)));
    menuButton->LockedSprite = MenuEntriesLSprites[i];

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
  if (ScrWork[SW_SYSMENUCT] < 32 && State == Shown) {
    Hide();
  }

  if (ScrWork[SW_SYSMENUCT] == 32 && FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_SYSMENUCT] == 0 && FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}

void SystemMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSMENUALPHA] > 0) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(SystemMenuBackgroundSprite,
                         glm::vec2(SystemMenuX, SystemMenuY), col);
    MainItems->Tint = col;
    MainItems->Render();
  }
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto