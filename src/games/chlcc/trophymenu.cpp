#include "trophymenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/ui/trophymenu.h"
#include "../../profile/games/chlcc/trophymenu.h"
#include "../../profile/games/chlcc/commonmenu.h"

#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../data/achievementsystem.h"
#include "../../ui/widgets/chlcc/trophymenuentry.h"

#include <numbers>

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::CommonMenu;

using namespace Impacto::Profile::TrophyMenu;
using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::GameSpecific;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CHLCC;

TrophyMenu::TrophyMenu() : CommonMenu(true) {
  TrophyCountHintLabel.Enabled = false;
  TrophyCountHintLabel.MoveTo(TrophyCountHintLabelPos);
}

void TrophyMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      FromSystemMenuTransition->StartIn();
    }
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;

    for (size_t i = 0; i < MaxTrophyPages; i++) {
      for (size_t j = 0; j < EntriesPerPage; j++) {
        const size_t index = i * EntriesPerPage + j;
        if (index >= AchievementSystem::GetAchievementCount()) break;
        TrophyMenuEntry* entry = new TrophyMenuEntry(static_cast<int>(index));
        MainItems[i].Add(entry);
      }
    }
    MainItems[CurrentPage].Show();
    if (!TrophyCountHintLabel.Enabled) {
      TrophyCountHintLabel.Enabled = true;
      TrophyCountHintLabel.SetText(
          Vm::ScriptGetTextTableStrAddress(TrophyCountHintTextTableId,
                                           TrophyCountHintStringNum),
          TrophyCountFontSize, RendererOutlineMode::Full, 0);
    }
  }
}
void TrophyMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
      FromSystemMenuTransition->StartOut();
    }
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void TrophyMenu::Render() {
  if (State == Hidden) return;
  CommonMenu::DrawSubmenu(BackgroundColor, CircleSprite, MenuTitleText,
                          MenuTitleTextAngle, true);

  if (MenuTransition.Progress < 0.22f) return;

  glm::vec2 offset = MenuTransition.GetPageOffset();

  DrawButtonPrompt(ButtonPromptSprite, ButtonPromptPosition);

  TrophyCountHintLabel.Move(offset);
  TrophyCountHintLabel.Render();
  TrophyCountHintLabel.Move(-offset);

  Renderer->DrawSprite(PlatinumTrophySprite, offset + PlatinumTrophyPos);
  Renderer->DrawSprite(GoldTrophySprite, offset + GoldTrophyPos);
  Renderer->DrawSprite(SilverTrophySprite, offset + SilverTrophyPos);
  Renderer->DrawSprite(BronzeTrophySprite, offset + BronzeTrophyPos);

  Renderer->DrawSprite(TrophyPageCtBoxSprite, offset + TrophyPageCtPos);
  Renderer->DrawSprite(PageNums[CurrentPage + 1], offset + CurrentPageNumPos);
  Renderer->DrawSprite(PageNumSeparatorSlash, offset + PageNumSeparatorPos);
  Renderer->DrawSprite(ReachablePageNums[MaxTrophyPages],
                       offset + MaxPageNumPos);

  MainItems[CurrentPage].Move(offset);
  MainItems[CurrentPage].Render();
  MainItems[CurrentPage].Move(-offset);

  Renderer->DrawSprite(TrophyEntriesBorderSprite, offset);
}

void TrophyMenu::UpdateInput(float dt) {
  if (IsFocused) {
    if (PADinputButtonWentDown & PAD1DOWN || Input::MouseWheelDeltaY < 0 ||
        PADinputButtonWentDown & PADcustom[8]) {
      if (CurrentPage < 8) {
        MainItems[CurrentPage++].Hide();
        MainItems[CurrentPage].Show();
      }
    } else if (PADinputButtonWentDown & PAD1UP || Input::MouseWheelDeltaY > 0 ||
               PADinputButtonWentDown & PADcustom[7]) {
      if (CurrentPage > 0) {
        MainItems[CurrentPage--].Hide();
        MainItems[CurrentPage].Show();
      }
    }
  }
}

void TrophyMenu::Update(float dt) {
  UpdateInput(dt);

  const int sysMenuCt = ScrWork[SW_SYSMENUCT];
  const int systemMenuCHG = ScrWork[SW_SYSTEMMENUCHG];

  if ((!GetFlag(SF_ACHIEVEMENTMENU) || sysMenuCt < 10000 ||
       (sysMenuCt == 10000 && systemMenuCHG != 0 && systemMenuCHG != 64)) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_ACHIEVEMENTMENU) && sysMenuCt > 0 && State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && !GetFlag(SF_ACHIEVEMENTMENU) &&
      systemMenuCHG == 0 && (sysMenuCt == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
    for (int i = 0; i < 9; i++) {
      MainItems[i].Clear();
    }
  } else if (MenuTransition.IsIn() && sysMenuCt == 10000 &&
             (systemMenuCHG == 0 || systemMenuCHG == 64) &&
             GetFlag(SF_ACHIEVEMENTMENU) && State == Showing) {
    State = Shown;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    FromSystemMenuTransition->Update(dt);
    if (MenuTransition.Direction == AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    UpdateTitles(MenuTitleTextRightPosition, MenuTitleTextLeftPosition);
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto