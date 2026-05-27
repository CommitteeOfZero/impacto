#include "systemmenu.h"
#include <glm/common.hpp>
#include "../../profile/games/cclcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../profile/ui/systemmenu.h"
#include "../../profile/game.h"
#include "../../ui/widgets/cclcc/sysmenubutton.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::UI::Widgets::CCLCC;
using namespace Impacto::Input;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  bool targetButtonLocked = static_cast<SysMenuButton*>(target)->IsLocked;
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", targetButtonLocked ? 4 : 2, false,
                     0);
  if (targetButtonLocked) {
    // Yep, that's similar to how it's done in the binary
    // Binary checks for button state and if it's locked, PADone is modified
    // like button press never happened, and then script reads inputs
    PADinputButtonWentDown = PADinputButtonWentDown & ~PAD1A;
    PADinputMouseWentDown = PADinputMouseWentDown & ~PAD1A;
    return;
  }

  ScrWork[SW_SYSMENUCNO] = target->Id;
  ChoiceMade = true;
}

void SystemMenu::UpdateInput(float dt) {
  // prevents broken state for quick exiting from/entering to direct menus
  if (State == Hiding || State == Showing) {
    PADinputButtonWentDown = 0;
    PADinputMouseWentDown = 0;
  }

  if (!IsFocused) return;
  const auto* const prevSelected = CurrentlyFocusedElement;
  Menu::UpdateInput(dt);
  MainItems->UpdateInput(dt);
  if (CurrentlyFocusedElement && prevSelected != CurrentlyFocusedElement) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }
}

SystemMenu::SystemMenu() : CommonMenu(FadeAnimation) {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.DurationIn = MoveInDuration;
  MenuTransition.DurationOut = MoveOutDuration;

  ItemsFade.Direction = AnimationDirection::In;
  ItemsFade.LoopMode = AnimationLoopMode::Stop;
  ItemsFade.DurationIn = ItemsFadeInDuration;
  ItemsFade.DurationOut = ItemsFadeOutDuration;

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  MainItems = new Widgets::Group(this);

  ScreenCap.Sheet =
      SpriteSheet((float)Window->WindowWidth, (float)Window->WindowHeight);
  Texture tex;
  tex.LoadSolidColor(Window->WindowWidth, Window->WindowHeight, 0x000000);
  ScreenCap.Sheet.Texture = tex.Submit();
  ScreenCap.Bounds.Width = ScreenCap.Sheet.DesignWidth;
  ScreenCap.Bounds.Height = ScreenCap.Sheet.DesignHeight;

  for (int i = 0; i < MenuEntriesNum; i++) {
    SysMenuButton* menuButton = new SysMenuButton(
        i, MenuEntriesSprites[i], Sprite(), MenuEntriesHSprites[i],
        MenuEntriesPositions[i], MenuEntriesButtonBounds[i]);

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
    OpenedAsDirect = GetFlag(SF_SYSTEMMENUDIRECT);
    State = Showing;

    CommonMenu::OnShow(FadeInDuration, FadeOutDuration, FadeAnimation);
    MenuTransition.StartIn();
    FadeAnimation.StartIn();
    // If the function was called due to a submenu opening directly,
    // then don't take over focus
    if (!((ScrWork[SW_SYSMENUCT] == 32 && ScrWork[SW_SYSSUBMENUCT]) ||
          ScrWork[SW_CLRALPHA])) {
      if (UI::FocusedMenu != 0) {
        LastFocusedMenu = UI::FocusedMenu;
        LastFocusedMenu->IsFocused = false;
      }
      IsFocused = true;
      UI::FocusedMenu = this;
      ItemsFade.StartIn();
      MainItems->Show();
      if (LastFocusedButtonId && *LastFocusedButtonId < MenuEntriesNum) {
        CurrentlyFocusedElement = MainItems->Children[*LastFocusedButtonId];
        CurrentlyFocusedElement->HasFocus = true;
      } else if (!CurrentlyFocusedElement) {
        AdvanceFocus(FDIR_DOWN);
      }
    }
  }
}

void SystemMenu::Hide() {
  if (State != Hidden) {
    if (CurrentlyFocusedElement) {
      auto* btn = static_cast<Widgets::Button*>(CurrentlyFocusedElement);
      if (btn) {
        LastFocusedButtonId = btn->Id;
      }
    }
    State = Hiding;
    FadeAnimation.StartOut();
    MenuTransition.StartOut();
    ItemsFade.StartOut();
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
  UpdateInput(dt);

  if (State == Shown &&
      ((GetFlag(SF_TITLEMODE) || ScrWork[SW_SYSMENUCT] < 32) ||
       ScrWork[SW_SYSMENUALPHA] == 0)) {
    Hide();
    return;
  }
  if (State == Hidden && !GetFlag(SF_TITLEMODE) &&
      ((ScrWork[SW_SYSMENUCT] > 0) || ScrWork[SW_SYSMENUALPHA] > 0)) {
    Show();
    return;
  }
  if (State == Showing && ScrWork[SW_SYSMENUCT] == 32) {
    State = Shown;
    return;
  }
  if (State == Hiding && FadeAnimation.IsOut() && MenuTransition.IsOut() &&
      ItemsFade.IsOut() && ScrWork[SW_SYSMENUCT] == 0) {
    State = Hidden;
    MainItems->Hide();
    return;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    FadeAnimation.Update(dt);
    if (ItemsFade.IsIn() && ScrWork[SW_SYSSUBMENUCT] > 0 && State == Shown &&
        ItemsFadeComplete) {
      ItemsFade.StartOut();
      ItemsFadeComplete = false;
    } else if (ItemsFade.IsOut() && ScrWork[SW_SYSSUBMENUCT] < 32 &&
               State == Shown && ItemsFadeComplete) {
      ItemsFade.StartIn();
      ItemsFadeComplete = false;
    }
    ItemsFade.Update(dt);
    if (!ItemsFadeComplete) {
      if (ItemsFade.IsIn() && ScrWork[SW_SYSSUBMENUCT] == 0) {
        ItemsFadeComplete = true;
      } else if (ItemsFade.IsOut() && ScrWork[SW_SYSSUBMENUCT] == 32) {
        ItemsFadeComplete = true;
      }
    }

    bool savesDisabled = GetFlag(SF_SAVEDISABLE);
    bool noFreeSlots = SaveSystem::MaxSaveEntries ==
                       SaveSystem::Implementation->GetLockedQuickSaveCount();
    bool quickSaveLockState =
        savesDisabled || noFreeSlots || SaveSystem::HasQSavedOnCurrentLine();
    static_cast<UI::CCLCC::SysMenuButton*>(
        MainItems->Children[static_cast<size_t>(MenuItems::QuickSave)])
        ->IsLocked = quickSaveLockState;
    static_cast<UI::CCLCC::SysMenuButton*>(
        MainItems->Children[static_cast<size_t>(MenuItems::Save)])
        ->IsLocked = savesDisabled;
  }

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);

    if ((CurrentInputDevice == Device::Mouse ||
         CurrentInputDevice == Device::Touch) &&
        ((PADinputMouseWentDown & PAD1A))) {
      bool noButtonsHovered = true;
      for (auto child : MainItems->Children) {
        auto button = static_cast<UI::CCLCC::SysMenuButton*>(child);
        if (button->Hovered) {
          noButtonsHovered = false;
          break;
        }
      }

      if (noButtonsHovered) {
        PADinputMouseWentDown = PADinputMouseWentDown & ~PAD1A;
        PADinputButtonWentDown = PADinputButtonWentDown & ~PAD1A;
      }
    }
  }
}

void SystemMenu::Render() {
  if (State != Hidden && !GetFlag(SF_TITLEMODE)) {
    if (MenuTransition.IsIn()) {
    }
    glm::vec3 tint = {1.0f, 1.0f, 1.0f};
    // Alpha goes from 0 to 1 in half the time
    float alpha = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
    if (!OpenedAsDirect) {
      CommonMenu::DrawBgSprite<true>(State, FadeAnimation, ScreenCap);
      Renderer->DrawSprite(
          SystemMenuMask,
          RectF{0, 0, Profile::DesignWidth, Profile::DesignHeight},
          glm::vec4{tint, alpha});
    }

    MainItems->Tint =
        glm::vec4(tint, glm::smoothstep(0.0f, 1.0f, ItemsFade.Progress));
    MainItems->Render();
  }
}

void SystemMenu::Init() {
  BGPosition = {CALCrnd((int)BGRandPosRange.x), CALCrnd((int)BGRandPosRange.y)};
  SetFlag(SF_SYSTEMMENUCAPTURE, true);

  bool backlogLockState =
      GetFlag(SF_BACKLOG_NOLOG) || GetFlag(SF_MESREVDISABLE);
  static_cast<SysMenuButton*>(
      MainItems->Children[static_cast<size_t>(MenuItems::Backlog)])
      ->IsLocked = backlogLockState;

  // these flag need to be recalculated after loading a game
  bool noFreeSlots = SaveSystem::MaxSaveEntries ==
                     SaveSystem::Implementation->GetLockedQuickSaveCount();
  SetFlag(SF_SAVEALLPROTECTED, noFreeSlots);
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto