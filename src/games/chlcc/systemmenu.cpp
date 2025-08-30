#include "systemmenu.h"
#include "../../profile/games/chlcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
#include "../../profile/ui/systemmenu.h"
#include "../../ui/widgets/chlcc/systemmenuentrybutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::UI::Widgets::CHLCC;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_SYSMENUCNO] = target->Id;
  // Make the Id match the save menu mode (5th button would be Quick Load which
  // is case 0)
  UI::SaveMenuPtr->ActiveMenuType =
      SaveMenuPageType::_from_integral_nothrow(target->Id % 4);
  ChoiceMade = true;
}

SystemMenu::SystemMenu() {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.DurationIn = FadeInDuration;
  MenuTransition.DurationOut = FadeOutDuration;

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  MenuLoop.Direction = AnimationDirection::In;
  MenuLoop.LoopMode = AnimationLoopMode::Loop;
  MenuLoop.DurationIn = MenuLoopDuration;

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
    MenuLoop.StartIn();
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    if (!CurrentlyFocusedElement) AdvanceFocus(FDIR_DOWN);
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
  UpdateInput(dt);
  if ((!GetFlag(SF_SYSTEMMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_SYSTEMMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
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

    UpdateMenuLoop();
    UpdateTitles();
  }

  auto* btn = static_cast<Widgets::Button*>(CurrentlyFocusedElement);
  if (btn) {
    IndexOfActiveButton = btn->Id;
  }
  if (State != Hidden && IsFocused) {
    UpdateSmoothSelection(dt);
    MenuLoop.Update(dt);
    UpdateRunningSelectedLabel(dt);
    MainItems->UpdateInput(dt);
    MainItems->Update(dt);
  }
}

void SystemMenu::Render() {
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawQuad(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();

    glm::vec3 tint = {1.0f, 1.0f, 1.0f};
    // Alpha goes from 0 to 1 in half the time
    float alpha =
        MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
    Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         glm::vec4(tint, alpha));
    DrawRedBar();
    float yOffset = 0;
    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);
      const CornersQuad titleDest = MainMenuTitleText.ScaledBounds()
                                        .RotateAroundCenter(MenuTitleTextAngle)
                                        .Translate(RightTitlePos);
      Renderer->DrawSprite(MainMenuTitleText, titleDest);
    }
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.72f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -720.0f, 0.0f,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }
      DrawButtonPrompt();
      if (IndexOfActiveButton >= 0 && State != Hidden) {
        DrawRunningSelectedLabel(SelectionOffsetY +
                                 MenuRunningSelectedLabelPosition.y + yOffset);
      }

      Renderer->DrawSprite(Background,
                           glm::vec2(BackgroundPosition.x, yOffset));
      DrawSelectMenu(yOffset);
      Renderer->DrawSprite(MainMenuTitleText, LeftTitlePos,
                           glm::vec4(tint, TitleFade.Progress));
      Renderer->DrawSprite(MenuItemsLine,
                           glm::vec2(MenuItemsLinePosition.x, yOffset));
      if (IndexOfActiveButton >= 0 && State != Hidden) {
        Renderer->DrawSprite(
            MenuSelectionDot,
            glm::vec2(MenuSelectionDotPosition.x,
                      MenuSelectionDotPosition.y +
                          IndexOfActiveButton * MenuSelectionDotMultiplier +
                          +yOffset));
        Renderer->DrawSprite(
            MenuSelection,
            glm::vec2(MenuSelectionPosition.x,
                      MenuSelectionPosition.y + SelectionOffsetY + yOffset));
      }

      MainItems->MoveTo(glm::vec2(0, yOffset));
      MainItems->Tint = glm::vec4(tint, 1.0f);
      MainItems->Render();
    }
  }
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
  float progress = MenuTransition.Progress * FadeInDuration * 60.0f;
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
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * FadeInDuration * 60.0f;
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

inline void SystemMenu::DrawSelectMenu(float yOffset) {
  float alpha;
  for (int idx = 0; idx < SelectMenuHeaderCount; idx++) {
    alpha = 1.0f;
    if (MenuLoop.Progress < 0.046f * (idx + 1)) {
      alpha = (MenuLoop.Progress - 0.046f * idx) / 0.046f;
    }
    Renderer->DrawSprite(SelectMenuHeader[idx],
                         glm::vec2(SelectMenuHeaderPositions[idx].x,
                                   SelectMenuHeaderPositions[idx].y + yOffset),
                         glm::vec4(glm::vec3(1.0f), alpha));
  }
}

void SystemMenu::UpdateMenuLoop() {
  if (MenuLoop.Progress < 0.362F) {
    LeftTitlePos = glm::vec2(
        MenuTitleTextPosition.x,
        glm::mix(
            1.0f, 721.0f,
            1.01011f * std::sin(1.62223f * (MenuLoop.Progress * 2.7604561455F) +
                                3.152f) +
                1.01012f));
  } else if (MenuLoop.Progress > 0.637F) {
    LeftTitlePos = glm::vec2(
        MenuTitleTextPosition.x,
        glm::mix(-MainMenuTitleText.Bounds.Height, 1.0f,
                 1.01011f * std::sin(1.62223f *
                                         ((MenuLoop.Progress * 2.7604559169F) -
                                          1.774F) +
                                     3.152f) +
                     1.01012f));
  }
}

void SystemMenu::UpdateTitles() {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MainMenuLabelRightPosition;

  if (MenuTransition.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
}

inline void SystemMenu::DrawErin() {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = 801.0f;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void SystemMenu::DrawButtonPrompt() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(MenuButtonPrompt, MenuButtonPromptPosition);
  } else if (MenuTransition.Progress > 0.734f) {
    float x =
        MenuButtonPromptPosition.x - 2560.0f * (MenuTransition.Progress - 1);
    Renderer->DrawSprite(MenuButtonPrompt,
                         glm::vec2(x, MenuButtonPromptPosition.y));
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto