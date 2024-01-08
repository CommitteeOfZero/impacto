#include "trophymenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/ui/trophymenu.h"
#include "../../profile/games/chlcc/trophymenu.h"
#include "../../io/memorystream.h"
#include "../../data/tipssystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::TrophyMenu;
using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

TrophyMenu::TrophyMenu() {
  MenuTransition.Direction = 1.0f;
  MenuTransition.LoopMode = ALM_Stop;
  MenuTransition.DurationIn = MenuTransitionDuration;
  MenuTransition.DurationOut = MenuTransitionDuration;

  TitleFade.Direction = 1.0f;
  TitleFade.LoopMode = ALM_Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;
}

void TrophyMenu::Show() {
  if (State != Shown) {
    if (State != Showing) MenuTransition.StartIn();
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void TrophyMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
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
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawRect(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();
    DrawRedBar();
  }
  if (MenuTransition.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);
    Renderer->DrawSprite(MenuTitleText, RightTitlePos, glm::vec4(1.0f),
                         glm::vec2(1.0f), MenuTitleTextAngle);
    Renderer->DrawSprite(MenuTitleText, LeftTitlePos);
  }

  glm::vec3 tint = {1.0f, 1.0f, 1.0f};
  // Alpha goes from 0 to 1 in half the time
  float alpha =
      MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
  Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                       glm::vec4(tint, alpha));

  glm::vec2 offset(0.0f, 0.0f);
  if (MenuTransition.Progress > 0.22f) {
    if (MenuTransition.Progress < 0.72f) {
      // Approximated function from the original, another mess
      offset = glm::vec2(
          0.0f,
          glm::mix(-720.0f, 0.0f,
                   1.00397f * std::sin(3.97161f -
                                       3.26438f * MenuTransition.Progress) -
                       0.00295643f));
    }
    DrawButtonPrompt();
  }
}

void TrophyMenu::Update(float dt) {
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_ACHIEVEMENTMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && State == Hiding)
    State = Hidden;
  else if (MenuTransition.IsIn() && State == Showing) {
    State = Shown;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == -1.0f && MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == 1.0f || TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    UpdateTitles();
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