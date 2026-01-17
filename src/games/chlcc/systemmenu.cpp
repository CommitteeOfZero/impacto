#include "systemmenu.h"
#include "../../profile/games/chlcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../profile/ui/systemmenu.h"
#include "../../ui/widgets/chlcc/systemmenuentrybutton.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

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

SystemMenu::SystemMenu() {
  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

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
    ShowMenu.StartIn();
    SelectAnimation.StartIn();
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;

    if (LastFocusedButtonId && *LastFocusedButtonId < MenuEntriesNum) {
      CurrentlyFocusedElement = MainItems->Children[*LastFocusedButtonId];
      CurrentlyFocusedElement->HasFocus = true;
    } else if (!CurrentlyFocusedElement) {
      AdvanceFocus(FDIR_DOWN);
    }
  }

  bool noFreeSlots = SaveSystem::MaxSaveEntries ==
                     SaveSystem::Implementation->GetLockedQuickSaveCount();
  SetFlag(SF_SAVEALLPROTECTED, noFreeSlots);
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
    ShowMenu.StartOut();
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
  if ((!GetFlag(SF_SYSTEMMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_SYSTEMMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (ShowMenu.IsOut() && ScrWork[SW_SYSMENUCT] == 0 && State == Hiding) {
    MainItems->Hide();
    State = Hidden;
    if (CurrentlyFocusedElement) {
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement = nullptr;
    }
  } else if (ShowMenu.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
  }

  if (State != Hidden) {
    ShowMenu.Update(dt);
    if (ShowMenu.Direction == AnimationDirection::Out &&
        ShowMenu.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (ShowMenu.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);

    UpdateMenuLoop();
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
  if (State != Hidden && IsFocused) {
    UpdateSmoothSelection(dt);
    SelectAnimation.Update(dt);
    UpdateRunningSelectedLabel(dt);
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

void SystemMenu::Render() {
  if (State == Hidden) return;

  if (ShowMenu.IsIn()) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        RgbIntToFloat(BackgroundColor));
  } else {
    DrawCircles();
  }
  DrawErin();

  glm::vec3 tint = {1.0f, 1.0f, 1.0f};
  // Alpha goes from 0 to 1 in half the time
  float alpha = ShowMenu.Progress < 0.5f ? ShowMenu.Progress * 2.0f : 1.0f;
  Renderer->DrawSprite(
      BackgroundFilter,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(tint, alpha));
  DrawRedBar();
  if (ShowMenu.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);
    const CornersQuad titleDest = MainMenuTitleText.ScaledBounds()
                                      .RotateAroundCenter(MenuTitleTextAngle)
                                      .Translate(RightTitlePos);
    Renderer->DrawSprite(MainMenuTitleText, titleDest);
  }

  if (ShowMenu.Progress < 0.22f) return;

  glm::vec2 offset = ShowMenu.GetPageOffset();
  DrawButtonPrompt();
  if (IndexOfActiveButton >= 0 && State != Hidden) {
    DrawRunningSelectedLabel(SelectionOffsetY +
                             MenuRunningSelectedLabelPosition.y + offset.y);
  }

  Renderer->DrawSprite(Background, glm::vec2(BackgroundPosition.x, offset.y));
  SelectAnimation.Draw(SelectMenuHeader, SelectMenuHeaderPositions, offset);
  Renderer->DrawSprite(MainMenuTitleText, LeftTitlePos,
                       glm::vec4(tint, TitleFade.Progress));
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

inline void SystemMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = ShowMenu.Progress * FadeInDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        Renderer->DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

inline void SystemMenu::DrawRedBar() {
  if (ShowMenu.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (ShowMenu.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = ShowMenu.Progress * FadeInDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}

void SystemMenu::UpdateMenuLoop() {
  // it uses same Animation for progress
  if (SelectAnimation.Progress < 0.362f) {
    LeftTitlePos = glm::vec2(
        MenuTitleTextPosition.x,
        glm::mix(1.0f, 721.0f,
                 1.01011f * std::sin(1.62223f * (SelectAnimation.Progress *
                                                 2.7604561455F) +
                                     3.152f) +
                     1.01012f));
  } else if (SelectAnimation.Progress > 0.637f) {
    LeftTitlePos = glm::vec2(
        MenuTitleTextPosition.x,
        glm::mix(-MainMenuTitleText.Bounds.Height, 1.0f,
                 1.01011f * std::sin(1.62223f * ((SelectAnimation.Progress *
                                                  2.7604559169F) -
                                                 1.774F) +
                                     3.152f) +
                     1.01012f));
  }
}

void SystemMenu::UpdateTitles() {
  if (ShowMenu.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MainMenuLabelRightPosition;

  if (ShowMenu.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (ShowMenu.Progress * 4.0f - 3.0f),
                460.0f * (ShowMenu.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos += glm::vec2(-572.0f * (ShowMenu.Progress * 4.0f - 3.0f),
                             460.0f * (ShowMenu.Progress * 4.0f - 3.0f) / 3.0f);
}

inline void SystemMenu::DrawErin() {
  float y = ErinPosition.y;
  if (ShowMenu.Progress < 0.78f) {
    y = 801.0f;
    if (ShowMenu.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f - 0.998267f * sin(3.97835f - 3.27549f * ShowMenu.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void SystemMenu::DrawButtonPrompt() {
  if (ShowMenu.IsIn()) {
    Renderer->DrawSprite(MenuButtonPrompt, MenuButtonPromptPosition);
  } else if (ShowMenu.Progress > 0.734f) {
    float x = MenuButtonPromptPosition.x - 2560.0f * (ShowMenu.Progress - 1);
    Renderer->DrawSprite(MenuButtonPrompt,
                         glm::vec2(x, MenuButtonPromptPosition.y));
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto