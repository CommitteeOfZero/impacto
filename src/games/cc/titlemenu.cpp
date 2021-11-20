#include "titlemenu.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/cc/titlemenu.h"
#include "../../renderer2d.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../audio/audiosystem.h"
#include "../../audio/audiostream.h"
#include "../../audio/audiochannel.h"
#include "../../io/vfs.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace CC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR1] = target->Id;
  ChoiceMade = true;
}

void TitleMenu::SecondaryButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR2] = target->Id;
  ChoiceMade = true;
}

TitleMenu::TitleMenu() {}

void TitleMenu::Show() {
  if (State == Hidden) {
    State = Shown;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    if (PressToStartAnimation.State == AS_Stopped) {
      PressToStartAnimation.StartIn();
      SmokeAnimation.StartIn();
    }
  }
}
void TitleMenu::Hide() {
  if (State == Shown) {
    State = Hidden;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput();

  PressToStartAnimation.Update(dt);
  SmokeAnimation.Update(dt);
  MoveLeftAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {
        PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
        PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
      } break;
      case 2: {
        PressToStartAnimation.DurationIn = PressToStartAnimFastDurationIn;
        PressToStartAnimation.DurationOut = PressToStartAnimFastDurationOut;
      } break;
      case 3: {
        if (MoveLeftAnimation.IsOut() && ScrWork[SW_TITLEDISPCT] > 0) {
          MoveLeftAnimation.StartIn();
        } else if (MoveLeftAnimation.IsIn() && ScrWork[SW_TITLEDISPCT] < 90) {
          MoveLeftAnimation.StartOut();
        }
      } break;
    }
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {  // Press to start
        DrawMainBackground();
        DrawStartButton();
        Renderer2D::DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        DrawSmoke(SmokeOpacityNormal);
        Renderer2D::DrawRect(
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
            glm::vec4(1.0f, 1.0f, 1.0f,
                      1.0f - ScrWork[SW_TITLEDISPCT] / 60.0f));
      } break;
      case 2: {  // Transition between Press to start and menus
        DrawMainBackground();
        DrawStartButton();
        Renderer2D::DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        DrawSmoke(SmokeOpacityNormal);
      } break;
      case 3: {  // Main Menu Fade In
        float backgroundBoundsY = BackgroundBoundsYNormal,
              fenceBoundsY = FenceBoundsYNormal;
        if (GetFlag(SF_CLR_TRUE_CC)) {
          backgroundBoundsY = BackgroundBoundsYTrue;
          fenceBoundsY = FenceBoundsYTrue;
        }
        BackgroundSprite.Bounds = RectF(
            BackgroundBoundsX -
                (BackgroundBoundsX * MoveLeftAnimation.Progress),
            backgroundBoundsY, BackgroundBoundsWidth, BackgroundBoundsHeight);
        FenceSprite.Bounds =
            RectF(FenceBoundsX - (FenceBoundsX * MoveLeftAnimation.Progress),
                  fenceBoundsY, FenceBoundsWidth, FenceBoundsHeight);
        Renderer2D::DrawSprite(BackgroundSprite,
                               glm::vec2(BackgroundX, BackgroundY));
        Renderer2D::DrawSprite(FenceSprite, glm::vec2(FenceX, FenceY));
        Renderer2D::DrawSprite(
            CopyrightSprite,
            glm::vec2(CopyrightX +
                          (CopyrightXMoveOffset * MoveLeftAnimation.Progress),
                      CopyrightY));
        Renderer2D::DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        if (!GetFlag(SF_CLR_TRUE_CC)) {
          DrawSmoke(SmokeOpacityNormal);
        }
      } break;
      case 4: {
      } break;
      case 7:
      case 8: {
      } break;
      case 11: {  // Initial Fade In
        DrawMainBackground(ScrWork[SW_TITLEDISPCT] / 32.0f);
        Renderer2D::DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        DrawSmoke(ScrWork[SW_TITLEDISPCT] / 128.0f);
      } break;
      case 12: {
      } break;
    }
  }
}

inline void TitleMenu::DrawMainBackground(float opacity) {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = opacity;
  Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(BackgroundX, BackgroundY),
                         col);
  Renderer2D::DrawSprite(FenceSprite, glm::vec2(FenceX, FenceY), col);
  Renderer2D::DrawSprite(CopyrightSprite, glm::vec2(CopyrightX, CopyrightY),
                         col);
}

inline void TitleMenu::DrawStartButton() {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
  Renderer2D::DrawSprite(PressToStartSprite,
                         glm::vec2(PressToStartX, PressToStartY), col);
}

inline void TitleMenu::DrawSmoke(float opacity) {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = opacity;
  SmokeSprite.Bounds = RectF(
      SmokeBoundsWidth - (SmokeAnimationBoundsXMax * SmokeAnimation.Progress) +
          SmokeAnimationBoundsXOffset,
      SmokeBoundsY,
      SmokeBoundsWidth -
          (SmokeAnimationBoundsXMax * (1.0f - SmokeAnimation.Progress)),
      SmokeBoundsHeight);
  Renderer2D::DrawSprite(SmokeSprite, glm::vec2(SmokeX, SmokeY), col);
  SmokeSprite.Bounds = RectF(
      SmokeBoundsX, SmokeBoundsY,
      SmokeBoundsWidth - (SmokeAnimationBoundsXMax * SmokeAnimation.Progress),
      SmokeBoundsHeight);
  Renderer2D::DrawSprite(
      SmokeSprite,
      glm::vec2(SmokeBoundsWidth - (SmokeAnimationBoundsXMax *
                                    (1.0f - SmokeAnimation.Progress)),
                SmokeY),
      col);
}

}  // namespace CC
}  // namespace UI
}  // namespace Impacto