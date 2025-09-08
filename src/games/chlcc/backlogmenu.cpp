#include "backlogmenu.h"

#include "../../profile/games/chlcc/backlogmenu.h"
#include "../../ui/widgets/chlcc/backlogentry.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/tipssystem.h"
#include "../../background2d.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::BacklogMenu;
using namespace Impacto::UI::Widgets::CHLCC;
using namespace Impacto::Profile::ScriptVars;

BacklogMenu::BacklogMenu() {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.DurationIn = TransitionDuration;
  MenuTransition.DurationOut = TransitionDuration;

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
}

void BacklogMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      FromSystemMenuTransition.StartIn();
    }
    IsFocused = true;
    UI::BacklogMenu::Show();
  }
}

void BacklogMenu::Hide() {
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
    Audio::Channels[Audio::AC_REV]->Stop(0.0f);
  }
}

void BacklogMenu::Render() {
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

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

      const CornersQuad titleDest = MenuTitleText.ScaledBounds()
                                        .RotateAroundCenter(MenuTitleTextAngle)
                                        .Translate(RightTitlePos);
      Renderer->DrawSprite(MenuTitleText, titleDest);
    }

    Renderer->CaptureScreencap(ShaderScreencapture.BgSprite);
    Renderer->DrawCHLCCMenuBackground(
        ShaderScreencapture.BgSprite, BackgroundFilter,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        MenuTransition.Progress);

    float yOffset = 0;
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.73f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -Profile::DesignHeight, 0.0f,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }
      Renderer->DrawSprite(BacklogBackgroundSprite, {0.0f, 0.0f + yOffset});
      DrawButtonPrompt();

      MainItems->RenderingBounds.Y += yOffset;
      MainItems->Move({0.0f, yOffset});
      MainItems->Render();
      MainItems->Move({0.0f, -yOffset});
      MainItems->RenderingBounds.Y -= yOffset;

      MainScrollbar->Move({0.0f, yOffset});
      MainScrollbar->Render();
      MainScrollbar->Move({0.0f, -yOffset});
    }
    if (MenuTransition.Progress > 0.34f) {
      Renderer->EnableScissor();
      Renderer->SetScissorRect(BacklogBackgroundSprite.Bounds);
      Renderer->DrawSprite(MenuTitleText, LeftTitlePos);
      Renderer->DisableScissor();
      RenderHighlight();
    }
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.73f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -Profile::DesignHeight, 0.0f,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }
      MainItems->RenderingBounds.Y += yOffset;
      MainItems->Move({0.0f, yOffset});
      MainItems->Render();
      MainItems->Move({0.0f, -yOffset});
      MainItems->RenderingBounds.Y -= yOffset;

      MainScrollbar->Move({0.0f, yOffset});
      MainScrollbar->Render();
      MainScrollbar->Move({0.0f, -yOffset});
    }
  }
}

void BacklogMenu::Update(float dt) {
  if ((!GetFlag(SF_BACKLOGMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_BACKLOGMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() &&
      (ScrWork[SW_SYSMENUCT] == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
    MainItems->Hide();
  } else if (MenuTransition.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
    MainItems->HasFocus = true;
  }

  if (State != Hidden) {
    UI::BacklogMenu::Update(dt);
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
  }
}

inline void BacklogMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * TransitionDuration * 60.0f;
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

inline void BacklogMenu::DrawErin() {
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

inline void BacklogMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * TransitionDuration * 60.0f;
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

inline void BacklogMenu::DrawButtonPrompt() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (MenuTransition.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (MenuTransition.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
  }
}

void BacklogMenu::UpdateTitles() {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MenuTitleTextRightPosition;
  LeftTitlePos =
      glm::vec2(TitleFade.IsIn()
                    ? MenuTitleTextLeftPosition.x
                    : 1477 - 1400 * std::sin(TitleFade.Progress *
                                             std::numbers::pi_v<float> * 0.5f),
                MenuTitleTextLeftPosition.y);

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
