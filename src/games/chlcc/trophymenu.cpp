#include "trophymenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/trophymenu.h"
#include "../../profile/games/chlcc/trophymenu.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../data/tipssystem.h"
#include "../../data/achievementsystem.h"
#include "trophymenuentry.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::TrophyMenu;
using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CHLCC;

TrophyMenu::TrophyMenu() {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.SetDuration(MenuTransitionDuration);

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  FromSystemMenuTransition.Direction = AnimationDirection::In;
  FromSystemMenuTransition.LoopMode = AnimationLoopMode::Stop;
  FromSystemMenuTransition.DurationIn = TitleFadeInDuration;
  FromSystemMenuTransition.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  TrophyCountHintLabel.Enabled = false;
}

void TrophyMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      FromSystemMenuTransition.StartIn();
    }
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;

    for (size_t i = 0; i < MaxTrophyPages; i++) {
      for (size_t j = 0; j < 6; j++) {
        const size_t index = i * 6 + j;
        if (index >= AchievementSystem::GetAchievementCount()) break;
        TrophyMenuEntry* entry = new TrophyMenuEntry(static_cast<int>(index));
        MainItems[i].Add(entry);
      }
    }
    Offset = glm::vec2(0.0f, -Profile::DesignHeight);
    MainItems[CurrentPage].Show();
    if (!TrophyCountHintLabel.Enabled) {
      TrophyCountHintLabel.Enabled = true;
      TrophyCountHintLabel.SetText(Vm::ScriptGetTextTableStrAddress(0, 20), 20,
                                   RendererOutlineMode::Full, 0);
    }
  }
}
void TrophyMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
      FromSystemMenuTransition.StartOut();
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
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawQuad(
          RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
          RgbIntToFloat(BackgroundColor));
    } else if (GetFlag(SF_SYSTEMMENU)) {
      Renderer->DrawQuad(
          RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
          RgbIntToFloat(BackgroundColor, FromSystemMenuTransition.Progress));
    } else {
      DrawCircles();
    }
    DrawErin();
    DrawRedBar();
  }
  if (MenuTransition.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

    const CornersQuad titleDest = MenuTitleText.ScaledBounds()
                                      .RotateAroundCenter(MenuTitleTextAngle)
                                      .Translate(RightTitlePos);
    Renderer->DrawSprite(MenuTitleText, titleDest);

    Renderer->DrawSprite(MenuTitleText, LeftTitlePos);
  }

  glm::vec3 tint = {1.0f, 1.0f, 1.0f};
  // Alpha goes from 0 to 1 in half the time
  float alpha =
      MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
  Renderer->DrawSprite(
      BackgroundFilter,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(tint, alpha));

  if (MenuTransition.Progress > 0.22f) {
    if (MenuTransition.Progress < 0.72f) {
      // Approximated function from the original, another mess
      Offset = glm::vec2(
          0.0f,
          glm::mix(-Profile::DesignHeight, 0.0f,
                   1.00397f * std::sin(3.97161f -
                                       3.26438f * MenuTransition.Progress) -
                       0.00295643f));
    }
    DrawButtonPrompt();
  }
  TrophyCountHintLabel.Render();
  Renderer->DrawSprite(PlatinumTrophySprite, Offset + PlatinumTrophyPos);
  Renderer->DrawSprite(GoldTrophySprite, Offset + GoldTrophyPos);
  Renderer->DrawSprite(SilverTrophySprite, Offset + SilverTrophyPos);
  Renderer->DrawSprite(BronzeTrophySprite, Offset + BronzeTrophyPos);

  Renderer->DrawSprite(TrophyPageCtBoxSprite,
                       Offset + glm::vec2(1090.0f, 60.0f));
  Renderer->DrawSprite(PageNums[CurrentPage + 1], Offset + CurrentPageNumPos);
  Renderer->DrawSprite(PageNumSeparatorSlash, Offset + PageNumSeparatorPos);
  Renderer->DrawSprite(ReachablePageNums[MaxTrophyPages],
                       Offset + MaxPageNumPos);

  Renderer->DrawSprite(TrophyEntriesBorderSprite, Offset);
  MainItems[CurrentPage].Render();
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

  if ((!GetFlag(SF_ACHIEVEMENTMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_ACHIEVEMENTMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() &&
      (ScrWork[SW_SYSMENUCT] == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
    for (int i = 0; i < 9; i++) {
      MainItems[i].Clear();
    }
  } else if (MenuTransition.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    FromSystemMenuTransition.Update(dt);
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
    for (auto* entry : MainItems[CurrentPage].Children) {
      TrophyMenuEntry* trophyEntry = static_cast<TrophyMenuEntry*>(entry);
      trophyEntry->UpdateOffset(Offset);
    }
    TrophyCountHintLabel.MoveTo(Offset + TrophyCountHintLabelPos);
  }
}

inline void TrophyMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        scale *= CircleSprite.Bounds.Height / 106.0f;
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

inline void TrophyMenu::DrawErin() {
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

inline void TrophyMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
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

inline void TrophyMenu::DrawButtonPrompt() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (MenuTransition.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (MenuTransition.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
  }
}

void TrophyMenu::UpdateTitles() {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MenuTitleTextRightPosition;
  LeftTitlePos = glm::vec2(
      MenuTitleTextLeftPosition.x,
      TitleFade.IsIn()
          ? MenuTitleTextLeftPosition.y
          : glm::mix(
                1.0f, 721.0f,
                1.01011f * std::sin(1.62223f * TitleFade.Progress + 3.152f) +
                    1.01012f));

  if (MenuTransition.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto