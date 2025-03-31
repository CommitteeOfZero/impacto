#include "optionsmenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/chlcc/optionsmenu.h"
#include "../../io/memorystream.h"
#include "../../data/tipssystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::CHLCC::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

OptionsMenu::OptionsMenu() : UI::OptionsMenu() {
  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  // Push dummy group so Show and Hide don't access garbage
  // Will be replaced when actually implemented
  auto dummyPage = std::make_unique<Group>(this);
  dummyPage->Add(new Label(), FDIR_DOWN);
  Pages.push_back(std::move(dummyPage));
}

void OptionsMenu::Render() {
  if (State != Hidden) {
    if (FadeAnimation.IsIn()) {
      Renderer->DrawRect(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();
    DrawRedBar();
  }
  if (FadeAnimation.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);
    Renderer->DrawSprite(MenuTitleText, RightTitlePos, glm::vec4(1.0f),
                         glm::vec2(1.0f), MenuTitleTextAngle);
  }

  glm::vec3 tint = {1.0f, 1.0f, 1.0f};
  // Alpha goes from 0 to 1 in half the time
  float alpha =
      FadeAnimation.Progress < 0.5f ? FadeAnimation.Progress * 2.0f : 1.0f;
  Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                       glm::vec4(tint, alpha));

  glm::vec2 offset(0.0f, 0.0f);
  if (FadeAnimation.Progress > 0.22f) {
    if (FadeAnimation.Progress < 0.72f) {
      // Approximated function from the original, another mess
      offset = glm::vec2(
          0.0f,
          glm::mix(-720.0f, 0.0f,
                   1.00397f * std::sin(3.97161f -
                                       3.26438f * FadeAnimation.Progress) -
                       0.00295643f));
    }
    DrawButtonPrompt();
  }
}

void OptionsMenu::UpdateVisibility() {
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_OPTIONMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (FadeAnimation.IsOut() && State == Hiding)
    State = Hidden;
  else if (FadeAnimation.IsIn() && State == Showing) {
    State = Shown;
  }
}

void OptionsMenu::Update(float dt) {
  UpdateVisibility();

  if (State != Hidden) {
    FadeAnimation.Update(dt);
    if (FadeAnimation.Direction == +AnimationDirection::Out &&
        FadeAnimation.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (FadeAnimation.IsIn() &&
               (TitleFade.Direction == +AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    UpdateTitles();
  }

  if (GetControlState(CT_Back)) {
    SetFlag(SF_SUBMENUEXIT, true);
  }
}

inline void OptionsMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = FadeAnimation.Progress * FadeInDuration * 60.0f;
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

inline void OptionsMenu::DrawErin() {
  float y = ErinPosition.y;
  if (FadeAnimation.Progress < 0.78f) {
    y = 801.0f;
    if (FadeAnimation.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * FadeAnimation.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void OptionsMenu::DrawRedBar() {
  if (FadeAnimation.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (FadeAnimation.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = FadeAnimation.Progress * FadeInDuration * 60.0f;
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

inline void OptionsMenu::DrawButtonPrompt() {
  if (FadeAnimation.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (FadeAnimation.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (FadeAnimation.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
  }
}

void OptionsMenu::UpdateTitles() {
  if (FadeAnimation.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MenuTitleTextRightPosition;

  if (FadeAnimation.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (FadeAnimation.Progress * 4.0f - 3.0f),
                460.0f * (FadeAnimation.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (FadeAnimation.Progress * 4.0f - 3.0f),
                460.0f * (FadeAnimation.Progress * 4.0f - 3.0f) / 3.0f);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto