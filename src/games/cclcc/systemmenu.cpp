#include "systemmenu.h"
#include "../../profile/games/cclcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
#include "../../profile/ui/systemmenu.h"
#include "../../ui/widgets/cclcc/sysmenubutton.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::UI::Widgets::CCLCC;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_SYSMENUCNO] = target->Id;
  // Make the Id match the save menu mode (5th button would be Quick Load which
  // is case 0)
  ScrWork[SW_SAVEMENUMODE] = target->Id % 4;
  ChoiceMade = true;
}

SystemMenu::SystemMenu() {
  MenuTransition.Direction = 1;
  MenuTransition.LoopMode = ALM_Stop;
  MenuTransition.DurationIn = FadeInDuration;
  MenuTransition.DurationOut = FadeOutDuration;

  TitleFade.Direction = 1.0f;
  TitleFade.LoopMode = ALM_Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  auto onClick =
      std::bind(&SystemMenu::MenuButtonOnClick, this, std::placeholders::_1);

  MainItems = new Widgets::Group(this);

  for (int i = 0; i < MenuEntriesNum; i++) {
    SysMenuButton* menuButton =
        new SysMenuButton(i, MenuEntriesSprites[i], Sprite(),
                          MenuEntriesHSprites[i], MenuEntriesPositions[i]);

    menuButton->OnClickHandler = onClick;
    MainItems->Add(menuButton, FDIR_DOWN);
  }
  MainItems->Children[0]->SetFocus(MainItems->Children[MenuEntriesNum - 1],
                                   FDIR_UP);
  MainItems->Children[MenuEntriesNum - 1]->SetFocus(MainItems->Children[0],
                                                    FDIR_DOWN);
}
void SystemMenu::Show() {
  if (State != Shown) {
    State = Showing;
    MenuTransition.StartIn();
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
    MenuTransition.StartOut();
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
  MainItems->UpdateInput();

  if (GetFlag(SF_TITLEMODE) == 0 && ScrWork[SW_SYSMENUCT] >= 16 &&
      State == Hidden) {
    Show();
  } else if (ScrWork[SW_SYSMENUCT] < 16 && State == Shown) {
    Hide();
  }
  if (MenuTransition.IsIn())
    State = Shown;
  else if (MenuTransition.IsOut())
    State = Hidden;

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == -1.0f && MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == 1.0f || TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
  }

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}
void SystemMenu::Render() {
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
    }
    glm::vec3 tint = {1.0f, 1.0f, 1.0f};
    // Alpha goes from 0 to 1 in half the time
    float alpha =
        MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
    // Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f,
    // 720.0f),
    //                      glm::vec4(tint, alpha));
    float yOffset = 0;

    MainItems->Tint = glm::vec4(tint, 1.0f);
    MainItems->Render();
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto