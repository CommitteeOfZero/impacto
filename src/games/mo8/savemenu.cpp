#include "savemenu.h"

#include "../../profile/ui/savemenu.h"
#include "../../profile/games/mo8/savemenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../data/savesystem.h"
#include "../../vm/interface/input.h"
#include "../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace MO8 {

using namespace Impacto::Profile::SaveMenu;
using namespace Impacto::Profile::MO8::SaveMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

Widget* EntryGrid[RowsPerPage][EntriesPerRow];

void SaveMenu::MenuButtonOnClick(Widgets::Button* target) {
  if ((SaveSystem::GetSaveSatus(SaveSystem::SaveType::SaveFull, target->Id) !=
       0) ||
      ScrWork[SW_SYSSUBMENUNO] == 4) {
    ScrWork[SW_SAVEFILENO] = target->Id;
  }
}

SaveMenu::SaveMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  PageControls = new Group(this);
  PageControls->FocusLock = false;
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto nextPage = new Button(0, NextButtonSprite, NextButtonHighlightedSprite,
                             nullSprite, NextButtonPosition);
  auto previousPage =
      new Button(0, BackButtonSprite, BackButtonHighlightedSprite, nullSprite,
                 BackButtonPosition);
  PageControls->Add(nextPage);
  PageControls->Add(previousPage);
  PageControls->IsShown = true;
}

void SaveMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}

void SaveMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SaveMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  bool subMenuSave = ScrWork[SW_SYSSUBMENUNO] == 0 ||
                     ScrWork[SW_SYSSUBMENUNO] == 3 ||
                     ScrWork[SW_SYSSUBMENUNO] == 4;
  if (ScrWork[SW_SYSSUBMENUCT] < 16 && subMenuSave && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && subMenuSave && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_SYSSUBMENUCT] == 16 && subMenuSave && FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_SYSSUBMENUCT] == 0 && subMenuSave &&
           FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown) {
    PageControls->Update(dt);
    if (GetControlState(CT_Back)) {
      SetFlag(SF_SUBMENUEXIT, true);
    }
  }
}

void SaveMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(SaveMenuBackgroundSprite, glm::vec2(0.0f), col);
    switch (ScrWork[SW_SYSSUBMENUNO]) {
      case 0:
        Renderer->DrawSprite(QuickLoadTextSprite, MenuTitleTextPos, col);
        break;
      case 3:
        Renderer->DrawSprite(SaveTextSprite, MenuTitleTextPos, col);
        break;
      case 4:
        Renderer->DrawSprite(LoadTextSprite, MenuTitleTextPos, col);
        break;
    }
    PageControls->Tint = col;
    PageControls->Render();
  }
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto