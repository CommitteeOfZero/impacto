#include "commonmenu.h"

#include "../../renderer/renderer.h"
#include "../../profile/games/chlcc/commonmenu.h"
#include "../../vm/interface/input.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::CommonMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

CommonMenu::CommonMenu(bool canGoFromSysMenu) {
  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  // FromSystemMenuTransition if nullopt if canGoFromSysMenu == false
  if (canGoFromSysMenu) {
    FromSystemMenuTransition = Animation();
    FromSystemMenuTransition->Direction = AnimationDirection::In;
    FromSystemMenuTransition->LoopMode = AnimationLoopMode::Stop;
    FromSystemMenuTransition->DurationIn = TitleFadeInDuration;
    FromSystemMenuTransition->DurationOut = TitleFadeOutDuration;
  }

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;
}

void CommonMenu::DrawSubmenu(uint32_t backgroundColor,
                             const Sprite& currentCircleSprite,
                             const Sprite& menuTitleSprite,
                             float menuTitleAngle, bool drawLeftTitle) {
  const bool fullDraw = !GetFlag(SF_SYSTEMMENU);
  // Those elements should not be drawn when sub menu is open from sys menu, sys
  // menu will draw them
  if (fullDraw) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawQuad(
          RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
          RgbIntToFloat(backgroundColor));
    } else {
      DrawCircles(currentCircleSprite);
    }

    DrawErin();
    DrawRedBar();
  }

  DrawRightTitle(menuTitleSprite, menuTitleAngle);
  if (fullDraw) {
    DrawBackgroundFilter();
  }
  // For menus with no left title or the when title is drawn over something else
  if (drawLeftTitle) {
    DrawLeftTitle(menuTitleSprite);
  }
}

void CommonMenu::DrawSysMenu(glm::vec4 backgroundColor,
                             const Sprite& currentCircleSprite,
                             const Sprite& menuTitleSprite,
                             float menuTitleAngle) {
  if (MenuTransition.IsIn()) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        backgroundColor);
  } else {
    DrawCircles(currentCircleSprite);
  }

  DrawErin();
  DrawRedBar();

  DrawRightTitle(menuTitleSprite, menuTitleAngle);
  DrawBackgroundFilter();
}

void CommonMenu::DrawCircles(const Sprite& currentCircleSprite) const {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
  const float scaleMultiplier = currentCircleSprite.ScaledHeight() / 106.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= progress) {
        float scale = (progress - (counter + 1.0f)) * 16.0f;
        scale = std::min(scale, 320.0f) * scaleMultiplier;
        Renderer->DrawSprite(
            currentCircleSprite,
            RectF(x + (currentCircleSprite.ScaledWidth() - scale) / 2.0f,
                  y + (currentCircleSprite.ScaledHeight() - scale) / 2.0f,
                  scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

void CommonMenu::DrawRedBar() const {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    const float progress =
        MenuTransition.Progress * MenuTransitionDuration * 60.0f;
    const float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    const float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}

void CommonMenu::DrawButtonPrompt(const Sprite& buttonPromptSprite,
                                  glm::vec2 buttonPromptPos,
                                  std::optional<Animation> animation) {
  const Animation promptAnimation = animation.value_or(MenuTransition);
  const float startProgress =
      ButtonPromptAnimationStartTime / MenuTransitionDuration;
  const float progressDuration =
      ButtonPromptAnimationDuration / MenuTransitionDuration;
  const float endProgress = startProgress + progressDuration;
  if (promptAnimation.Progress >= endProgress) {
    Renderer->DrawSprite(buttonPromptSprite, buttonPromptPos);
  } else if (promptAnimation.Progress >= startProgress) {
    const float progress =
        (promptAnimation.Progress - startProgress) / progressDuration;
    Renderer->DrawSprite(
        buttonPromptSprite,
        glm::mix(ButtonPromptStartPosition, buttonPromptPos, progress));
  }
}

void CommonMenu::DrawErin() const {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = ErinSprite.Bounds.Height;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, Profile::DesignHeight + ErinPosition.y,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

void CommonMenu::DrawRightTitle(const Sprite& titleSprite,
                                float titleAngle) const {
  if (MenuTransition.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

    const CornersQuad titleDest = titleSprite.ScaledBounds()
                                      .RotateAroundCenter(titleAngle)
                                      .Translate(RightTitlePos);
    Renderer->DrawSprite(titleSprite, titleDest);
  }
}

void CommonMenu::DrawLeftTitle(const Sprite& titleSprite,
                               glm::vec4 tint) const {
  if (MenuTransition.Progress > 0.34f) {
    Renderer->DrawSprite(titleSprite, LeftTitlePos, tint);
  }
}

void CommonMenu::UpdateTitles(glm::vec2 rightTitlepos, glm::vec2 leftTitlePos,
                              bool vertical) {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = rightTitlepos;
  LeftTitlePos = leftTitlePos;

  if (!TitleFade.IsIn()) {
    if (vertical) {
      const float delta =
          glm::mix(leftTitlePos.y, Profile::DesignHeight + leftTitlePos.y,
                   1.01011f * std::sin(1.62223f * TitleFade.Progress + 3.152f) +
                       1.01012f);
      LeftTitlePos.y += delta;
    } else {
      const float delta = glm::mix(
          Profile::DesignWidth, 0.0f,
          std::sin(TitleFade.Progress * std::numbers::pi_v<float> * 0.5f));
      LeftTitlePos.x += delta;
    }
  }

  if (MenuTransition.Progress >= 0.73f) return;

  const auto delta = glm::mix(DiagonalTitlesOffsetStart,
                              DiagonalTitlesOffsetEnd, MenuTransition.Progress);
  // RedTitleLabelPos should not update when submenu is open from sys menu
  if (!GetFlag(SF_SYSTEMMENU)) {
    RedTitleLabelPos += delta;
  }
  RightTitlePos += delta;
}

void CommonMenu::DrawBackgroundFilter() {
  Renderer->CaptureScreencap(ShaderScreencapture.BgSprite);
  Renderer->DrawCHLCCMenuBackground(
      ShaderScreencapture.BgSprite, BackgroundFilter,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      std::clamp(MenuTransition.Progress * 2.0f, 0.0f, 1.0f));
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto