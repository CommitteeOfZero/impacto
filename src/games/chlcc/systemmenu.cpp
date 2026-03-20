#include "systemmenu.h"

#include "../../profile/game.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../profile/ui/systemmenu.h"
#include "../../ui/widgets/chlcc/systemmenuentrybutton.h"

#include "../../profile/games/chlcc/systemmenu.h"
#include "../../profile/games/chlcc/commonmenu.h"
#include "../../profile/games/chlcc/backlogmenu.h"
#include "../../profile/games/chlcc/savemenu.h"
#include "../../profile/games/chlcc/optionsmenu.h"
#include "../../profile/games/chlcc/tipsmenu.h"
#include "../../profile/games/chlcc/trophymenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::CommonMenu;
using namespace Impacto::Profile::CHLCC::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::UI::Widgets::CHLCC;
using namespace Impacto::Input;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  if (static_cast<SystemMenuEntryButton*>(target)->IsLocked) {
    // Yep, that's similar to how it's done in the binary
    // Binary checks for button state and if it's locked, PADone is modified
    // like button press never happened, and then script reads inputs
    PADinputButtonWentDown = PADinputButtonWentDown & ~PAD1A;
    PADinputMouseWentDown = PADinputMouseWentDown & ~PAD1A;
    return;
  }

  ScrWork[SW_SYSMENUCNO] = target->Id;
  // Make the Id match the save menu mode (5th button would be Quick Load which
  // is case 0)
  UI::SaveMenuPtr->ActiveMenuType =
      SaveMenuPageType::_from_integral_nothrow(target->Id % 4);
  ChoiceMade = true;
}

SystemMenu::SystemMenu() : CommonMenu(false) {
  CurrentColor = Profile::CHLCC::SystemMenu::BackgroundColor;
  SubItemsTransition = MenuTransition;
  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  MainItems = new Widgets::Group(this);

  for (int i = 0; i < MenuEntriesNum; i++) {
    SystemMenuEntryButton* menuButton = new SystemMenuEntryButton(
        i, MenuEntriesSprites[i], MenuEntriesSprites[i],
        RgbIntToFloat(FocusTint), Sprite(), MenuEntriesPositions[i],
        RectF(
            MenuEntriesPositions[i].x - 30,
            (i + 1) * MenuSelectionDotMultiplier + MenuSelectionPosition.y - 14,
            300, 40));

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
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;

    SubItemShow();
  }

  bool noFreeSlots = SaveSystem::MaxSaveEntries ==
                     SaveSystem::Implementation->GetLockedQuickSaveCount();
  SetFlag(SF_SAVEALLPROTECTED, noFreeSlots);
}

void SystemMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    MenuTransition.StartOut();
    SubItemsHide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SystemMenu::SubItemsHide() {
  if (SubItemsState != Hidden) {
    SubItemsState = Hiding;
    SubItemsTransition.StartOut();
  }
  if (CurrentlyFocusedElement) {
    auto* btn = static_cast<Widgets::Button*>(CurrentlyFocusedElement);
    if (btn) {
      LastFocusedButtonId = btn->Id;
    }
  }
}
void SystemMenu::SubItemShow() {
  if (SubItemsState != Shown) {
    SubItemsState = Showing;
    SubItemsTransition.StartIn();
    MainItems->Show();
    SelectAnimation.StartIn(true);
  }
  if (LastFocusedButtonId && *LastFocusedButtonId < MenuEntriesNum) {
    CurrentlyFocusedElement = MainItems->Children[*LastFocusedButtonId];
    CurrentlyFocusedElement->HasFocus = true;
  } else if (!CurrentlyFocusedElement) {
    AdvanceFocus(FDIR_DOWN);
  }
}

void SystemMenu::Update(float dt) {
  UpdateInput(dt);
  const bool isSysMenuOpen = GetFlag(SF_SYSTEMMENU);
  if ((!isSysMenuOpen || ScrWork[SW_SYSMENUCT] < 10000) && State == Shown) {
    Hide();
  } else if (isSysMenuOpen && ScrWork[SW_SYSMENUCT] > 0 && State == Hidden) {
    Show();
  }

  if (isSysMenuOpen) {
    if (UI::FocusedMenu != this && SubItemsState == Shown &&
        UI::SysMesBoxPtr->State == UI::MenuState::Hidden) {
      SubItemsHide();
    } else if (UI::FocusedMenu == this && SubItemsState == Hidden) {
      SubItemShow();
    }
  }

  if (MenuTransition.IsOut() && ScrWork[SW_SYSMENUCT] == 0 && State == Hiding) {
    MainItems->Hide();
    State = Hidden;
    if (CurrentlyFocusedElement) {
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement = nullptr;
    }
  } else if (MenuTransition.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
  }

  if (SubItemsTransition.IsOut() && SubItemsState == Hiding) {
    MainItems->Hide();
    if (CurrentlyFocusedElement) {
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement = nullptr;
    }
    SubItemsState = Hidden;
  } else if (SubItemsTransition.IsIn() && SubItemsState == Showing) {
    SubItemsState = Shown;
  }

  if (SubItemsState != Hidden) {
    SubItemsTransition.Update(dt);
    UpdateRightTitle();
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);

    UpdateTitles();

    bool savesDisabled = GetFlag(SF_SAVEDISABLE);
    bool noFreeSlots = SaveSystem::MaxSaveEntries ==
                       SaveSystem::Implementation->GetLockedQuickSaveCount();
    bool quickSaveLockState =
        savesDisabled || SaveSystem::HasQSavedOnCurrentLine() || noFreeSlots;
    static_cast<SystemMenuEntryButton*>(
        MainItems->Children[static_cast<size_t>(MenuItems::QuickSave)])
        ->IsLocked = quickSaveLockState;
    static_cast<SystemMenuEntryButton*>(
        MainItems->Children[static_cast<size_t>(MenuItems::Save)])
        ->IsLocked = savesDisabled;
  }

  auto* btn = static_cast<Widgets::Button*>(CurrentlyFocusedElement);
  if (btn) {
    IndexOfActiveButton = btn->Id;
  }
  if (State != Hidden) {
    // These animations should update when sysmenu is unfocused (fading to
    // submenu/showing sysmesbox)
    UpdateSmoothSelection(dt);
    SelectAnimation.Update(dt);
    UpdateRunningSelectedLabel(dt);
    if (IsFocused) {
      MainItems->UpdateInput(dt);
      MainItems->Update(dt);

      if ((CurrentInputDevice == Device::Mouse ||
           CurrentInputDevice == Device::Touch) &&
          ((PADinputMouseWentDown & PAD1A))) {
        bool noButtonsHovered = true;
        for (auto child : MainItems->Children) {
          auto button = static_cast<SystemMenuEntryButton*>(child);
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
}

void SystemMenu::Render() {
  if (State == Hidden) return;
  CommonMenu::DrawSysMenu(GetCurrentBgColor(), CircleSprite, MainMenuTitleText,
                          MenuTitleTextAngle);
  if (SubItemsState == Hidden) return;

  if (MenuTransition.Progress < 0.22f) return;

  CommonMenu::DrawButtonPrompt(MenuButtonPrompt, MenuButtonPromptPosition,
                               SubItemsTransition);

  glm::vec2 offset = SubItemsTransition.GetPageOffset();
  if (IndexOfActiveButton >= 0 && State != Hidden) {
    DrawRunningSelectedLabel(SelectionOffsetY +
                             MenuRunningSelectedLabelPosition.y + offset.y);
  }

  Renderer->DrawSprite(Background, glm::vec2(BackgroundPosition.x, offset.y));
  SelectAnimation.Draw(SelectMenuHeader, SelectMenuHeaderPositions, offset);

  glm::vec3 tint = {1.0f, 1.0f, 1.0f};
  if (SubItemsState == Shown) {
    DrawLeftTitle(MainMenuTitleText, glm::vec4(tint, TitleFade.Progress));
  }
  Renderer->DrawSprite(MenuItemsLine,
                       glm::vec2(MenuItemsLinePosition.x, offset.y));
  if (IndexOfActiveButton >= 0 && State != Hidden) {
    Renderer->DrawSprite(
        MenuSelectionDot,
        glm::vec2(MenuSelectionDotPosition.x,
                  MenuSelectionDotPosition.y +
                      IndexOfActiveButton * MenuSelectionDotMultiplier +
                      +offset.y));
    Renderer->DrawSprite(
        MenuSelection,
        glm::vec2(MenuSelectionPosition.x,
                  MenuSelectionPosition.y + SelectionOffsetY + offset.y));
  }

  MainItems->MoveTo(offset);
  MainItems->Tint = glm::vec4(tint, 1.0f);
  MainItems->Render();
}

inline void SystemMenu::UpdateSmoothSelection(float dt) {
  float target = MenuSelectionDotMultiplier * IndexOfActiveButton;
  SelectionOffsetY += (target - SelectionOffsetY) * HoverLerpSpeed * dt;
}

inline void SystemMenu::UpdateRunningSelectedLabel(float dt) {
  CurrentRunningPosition += SelectedLabelSpeed * dt;
  if (glm::abs(CurrentRunningPosition) >=
      MenuRunningSelectedLabel.Bounds.Width) {
    CurrentRunningPosition = MenuRunningSelectedLabel.Bounds.Width -
                             glm::abs(CurrentRunningPosition);
  }
}

inline void SystemMenu::DrawRunningSelectedLabel(float offsetY) {
  float x = 0;
  for (int i = -1; i < (1280 / MenuRunningSelectedLabel.Bounds.Width) + 1;
       i++) {
    x = (i * (MenuRunningSelectedLabel.Bounds.Width - 3) +
         CurrentRunningPosition);
    Renderer->DrawSprite(
        MenuRunningSelectedLabel,
        glm::vec2(x, x * MenuRunningSelectedLabelAngle + offsetY));
  }
}

void SystemMenu::UpdateTitles() {
  // it also uses SelectAnimation for leftTitle
  if (SelectAnimation.Progress < 0.362f) {
    LeftTitlePos = glm::vec2(
        MenuTitleTextPosition.x,
        glm::mix(1.0f, Profile::DesignHeight + 1.0f,
                 1.01011f * std::sin(1.62223f * (SelectAnimation.Progress *
                                                 2.7604561455f) +
                                     3.152f) +
                     1.01012f));
  } else if (SelectAnimation.Progress > 0.637f) {
    LeftTitlePos = glm::vec2(
        MenuTitleTextPosition.x,
        glm::mix(-MainMenuTitleText.Bounds.Height, 1.0f,
                 1.01011f * std::sin(1.62223f * ((SelectAnimation.Progress *
                                                  2.7604559169f) -
                                                 1.774f) +
                                     3.152f) +
                     1.01012f));
  }

  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;

  if (MenuTransition.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::mix(DiagonalTitlesOffsetStart, DiagonalTitlesOffsetEnd,
               MenuTransition.Progress);
}

void SystemMenu::UpdateRightTitle() {
  if (SubItemsTransition.Progress <= 0.34f) return;

  RightTitlePos = MainMenuLabelRightPosition;

  if (SubItemsTransition.Progress >= 0.73f) return;
  RightTitlePos += glm::mix(DiagonalTitlesOffsetStart, DiagonalTitlesOffsetEnd,
                            SubItemsTransition.Progress);
}

glm::vec4 SystemMenu::GetCurrentBgColor() {
  const glm::vec4 sourceColor = RgbIntToFloat(BackgroundColor);
  if (UI::FocusedMenu != this) {
    if (GetFlag(SF_BACKLOGMENU)) {
      CurrentColor = Profile::CHLCC::BacklogMenu::BackgroundColor;
    } else if (GetFlag(SF_SAVEMENU)) {
      CurrentColor = Profile::CHLCC::SaveMenu::BackgroundColor;
    } else if (GetFlag(SF_OPTIONMENU)) {
      CurrentColor = Profile::CHLCC::OptionsMenu::BackgroundColor;
    } else if (GetFlag(SF_TIPSMENU)) {
      CurrentColor = Profile::CHLCC::TipsMenu::BackgroundColor;
    } else if (GetFlag(SF_ACHIEVEMENTMENU)) {
      CurrentColor = Profile::CHLCC::TrophyMenu::BackgroundColor;
    } else {
      CurrentColor = BackgroundColor;
    }
  }
  glm::vec4 targetColor = RgbIntToFloat(CurrentColor);
  // crossfading from one color to another
  return glm::mix(targetColor, sourceColor, SubItemsTransition.Progress);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto