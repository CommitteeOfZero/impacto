#include "clearlistmenu.h"

#include "../../profile/games/chlcc/clearlistmenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer2d.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"
#include "../../data/tipssystem.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::ClearListMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::UI::Widgets;
using namespace Impacto::TipsSystem;
using namespace Impacto::Profile;

ClearListMenu::ClearListMenu() {
  MenuTransition.Direction = 1;
  MenuTransition.LoopMode = ALM_Stop;
  MenuTransition.DurationIn = MenuTransitionDuration;
  MenuTransition.DurationOut = MenuTransitionDuration;

  TitleFade.Direction = 1;
  TitleFade.LoopMode = ALM_Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;
}

void ClearListMenu::Show() {
  // TODO: Fix not being able to return to the main menu

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

void ClearListMenu::Hide() {
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

void ClearListMenu::Render() {
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer2D::DrawRect(RectF(0.0f, 0.0f, 1280, 720),
                           RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();
    glm::vec3 tint = {1, 1, 1};
    // Alpha goes from 0 to 1 in half the time
    float alpha =
        MenuTransition.Progress < 0.5 ? MenuTransition.Progress * 2 : 1;
    Renderer2D::DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280, 720),
                           glm::vec4(tint, alpha));
    DrawRedBar();
    DrawTitles();
    int yOffset = 0;
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.72f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -720, 0,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }
      Renderer2D::DrawSprite(
          ClearListLabel,
          glm::vec2(LabelPosition.x, LabelPosition.y + yOffset));
      DrawPlayTime(yOffset);
      DrawEndingCount(yOffset);
      DrawTIPSCount(yOffset);
      DrawAlbumCompletion(yOffset);
      DrawEndingTree(yOffset);
      DrawButtonPrompt();
    }
  }
}

void ClearListMenu::Update(float dt) {
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_CLEARLISTMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
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
    if (MenuTransition.Direction == -1 && MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == 1 || TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
  }
}

inline void ClearListMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * MenuTransitionDuration * 60;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1)) * 16;
        scale = scale <= 320 ? scale : 320;
        scale *= CircleSprite.Bounds.Height / 106;
        Renderer2D::DrawSprite(
            CircleSprite,
            RectF(x + (CircleSprite.Bounds.Width - scale) / 2,
                  y + (CircleSprite.Bounds.Height - scale) / 2, scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

inline void ClearListMenu::DrawErin() {
  float y = 0;
  if (MenuTransition.Progress < 0.78f) {
    y = 800;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -20, 720,
          0.998938f -
              0.998267 * sin(3.97835f - 3.27549 * MenuTransition.Progress));
    }
  }
  Renderer2D::DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y + 1));
}

inline void ClearListMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer2D::DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * MenuTransitionDuration * 60;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47) / 17.0;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer2D::DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13 * (progress - 47);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer2D::DrawSprite(RedBarSprite, RedBarPosition);
  }
}

inline void ClearListMenu::DrawTitles() {
  if (MenuTransition.Progress > 0.34f) {
    float labelX = RedBarLabelPosition.x;
    float labelY = RedBarLabelPosition.y;
    float rightTitleX = MenuTitleTextRightPosition.x;
    float rightTitleY = MenuTitleTextRightPosition.y;
    float leftTitleY =
        glm::mix(1, 693,
                 1.04937f * std::sin(1.62531f * TitleFade.Progress + 3.14933f) +
                     1.0494f);
    if (MenuTransition.Progress < 0.72f) {
      labelX -= 572 * (MenuTransition.Progress * 4 - 3);
      rightTitleX -= 572 * (MenuTransition.Progress * 4 - 3);
      labelY += 460 * (MenuTransition.Progress * 4 - 3) / 3;
      rightTitleY += 460 * (MenuTransition.Progress * 4 - 3) / 3;
    }
    Renderer2D::DrawSprite(RedBarLabel, glm::vec2(labelX, labelY));
    Renderer2D::DrawSprite(MenuTitleText, glm::vec2(rightTitleX, rightTitleY),
                           glm::vec4(1.0f), glm::vec2(1.0f),
                           MenuTitleTextAngle);
    Renderer2D::DrawSprite(MenuTitleText,
                           glm::vec2(MenuTitleTextLeftPosition.x, leftTitleY));
  }
}

inline void ClearListMenu::DrawPlayTime(int yOffset) {
  int totalSeconds = ScrWork[SW_PLAYTIME];
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = (totalSeconds % 3600) % 60;

  if (hours > 99) {
    hours = 99;
    minutes = 59;
    seconds = 59;
  }
  int time[6] = {hours / 10,   hours % 10,   minutes / 10,
                 minutes % 10, seconds / 10, seconds % 10};

  for (int idx = 0; idx < 6; idx++) {
    if (!(idx % 2 == 0 && time[idx] == 0)) {
      glm::vec2 position(TimePositions[idx].x, TimePositions[idx].y + yOffset);
      Renderer2D::DrawSprite(Digits[time[idx]], position);
    }
  }
}

inline void ClearListMenu::DrawEndingCount(int yOffset) {
  int unlockedEndingCount = 0;
  for (int i = 0; i < Endings; i++) {
    unlockedEndingCount += GetFlag(SF_CLR_END1 + i);
  }
  glm::vec2 position(EndingCountPosition.x, EndingCountPosition.y + yOffset);
  Renderer2D::DrawSprite(Digits[unlockedEndingCount], position);
}

inline void ClearListMenu::DrawTIPSCount(int yOffset) {
  int unlockedTipsCount = 0;
  int totalTips = GetTipCount();
  for (int idx = 0; idx < totalTips; idx++) {
    unlockedTipsCount += GetTipLockedState(idx) ? 0 : 1;
  }
  if (unlockedTipsCount / 10 != 0) {
    Renderer2D::DrawSprite(
        Digits[unlockedTipsCount / 10],
        glm::vec2(TIPSCountPositions[0].x, TIPSCountPositions[0].y + yOffset));
  }
  Renderer2D::DrawSprite(
      Digits[unlockedTipsCount % 10],
      glm::vec2(TIPSCountPositions[1].x, TIPSCountPositions[1].y + yOffset));
}

inline void ClearListMenu::DrawAlbumCompletion(int yOffset) {
  int totalCount = 0, unlockedCount = 0;
  SaveSystem::GetViewedEVsCount(&totalCount, &unlockedCount);
  // The 9 bonus CGs after 100% completion don't count
  totalCount -= 9;
  unlockedCount = unlockedCount <= totalCount ? unlockedCount : totalCount;
  int percentage = unlockedCount * 100 / totalCount;
  if (percentage == 0 && (unlockedCount) != 0) {
    percentage = 1;
  }
  if (percentage / 100 != 0) {
    Renderer2D::DrawSprite(
        Digits[percentage / 100],
        glm::vec2(AlbumPositions[0].x, AlbumPositions[0].y + yOffset));
    Renderer2D::DrawSprite(
        Digits[(percentage / 10) % 10],
        glm::vec2(AlbumPositions[1].x, AlbumPositions[1].y + yOffset));
  } else if (percentage / 10 != 0) {
    Renderer2D::DrawSprite(
        Digits[(percentage / 10) % 10],
        glm::vec2(AlbumPositions[1].x, AlbumPositions[1].y + yOffset));
  }
  Renderer2D::DrawSprite(
      Digits[percentage % 10],
      glm::vec2(AlbumPositions[2].x, AlbumPositions[2].y + yOffset));
}

inline void ClearListMenu::DrawEndingTree(int yOffset) {
  for (int i = 0; i < 8; i++) {
    glm::vec2 boxPosition(BoxPositions[i].x, BoxPositions[i].y + yOffset);
    glm::vec2 thumbnailPosition(ThumbnailPositions[i].x,
                                ThumbnailPositions[i].y + yOffset);
    Renderer2D::DrawSprite(EndingBox, boxPosition);
    // Flag for the 1st ending, they are contiguous
    if (GetFlag(SF_CLR_END1 + i)) {
      Renderer2D::DrawSprite(EndingThumbnails[i], thumbnailPosition);
    } else {
      Renderer2D::DrawSprite(LockedThumbnail, thumbnailPosition);
    }
  }
  glm::vec2 listPosition(ListPosition.x, ListPosition.y + yOffset);
  Renderer2D::DrawSprite(EndingList, listPosition);
}

inline void ClearListMenu::DrawButtonPrompt() {
  if (MenuTransition.IsIn()) {
    Renderer2D::DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (MenuTransition.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560 * MenuTransition.Progress + 2561;
    Renderer2D::DrawSprite(ButtonPromptSprite,
                           glm::vec2(x, ButtonPromptPosition.y));
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto
