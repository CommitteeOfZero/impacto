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

  RedBarKickIn.Direction = 1;
  RedBarKickIn.LoopMode = ALM_Stop;
  RedBarKickIn.DurationIn = RedBarKickInTime;
  RedBarKickIn.StartIn();

  RedBarAnimation.Direction = 1;
  RedBarAnimation.LoopMode = ALM_Stop;
  RedBarAnimation.DurationIn = RedBarAnimationDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

}

void ClearListMenu::Show() {
  // TODO: Fix not being able to return to the main menu

  if (State != Shown) {
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
    //    Renderer2D::DrawRect(RectF(0.0f, 0.0f, 1280, 720),
    //                         RgbIntToFloat(BackgroundColor));
    //    Renderer2D::DrawSprite(ErinSprite, ErinPosition);
    //    Renderer2D::DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280, 720)); Renderer2D::DrawSprite(ClearListLabel, LabelPosition);
    //    DrawPlayTime(ScrWork[SW_PLAYTIME]);
    //    DrawEndingCount();
    //    DrawTIPSCount();
    //    DrawAlbumCompletion();
    //    DrawEndingTree();
    //    DrawRedBar();
    for (auto & CirclePosition : CirclePositions) {
      Renderer2D::DrawSprite(CircleSprite, CirclePosition);
    }
    Renderer2D::DrawThing(BackgroundFilter, RectF(0.0f, 0.0f, 1280, 720), 256, 1, RgbIntToFloat(BackgroundColor));
  }
}

void ClearListMenu::Update(float dt) {

  if (ScrWork[SW_PLAYDATA_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_PLAYDATA_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_PLAYDATA_ALPHA] == 256)
    State = Shown;
  else {
    State = Hidden;
  }

  if (State != Hidden) {
    RedBarKickIn.Update(dt);
    RedBarAnimation.Update(dt);
  }
}

inline void ClearListMenu::DrawPlayTime(int totalSeconds) {
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
    if (!(idx % 2 == 0 && time[idx] == 0))
      Renderer2D::DrawSprite(Digits[time[idx]], TimePositions[idx]);
  }
}

inline void ClearListMenu::DrawEndingCount() {
  int unlockedEndingCount = 0;
  for (int i = 0; i < Endings; i++) {
    unlockedEndingCount += GetFlag(SF_CLR_END1 + i);
  }
  Renderer2D::DrawSprite(Digits[unlockedEndingCount], EndingCountPosition);
}

inline void ClearListMenu::DrawTIPSCount() {
  int unlockedTipsCount = 0;
  int totalTips = GetTipCount();
  for (int idx = 0; idx < totalTips ; idx++) {
    unlockedTipsCount += GetTipLockedState(idx) ? 0 : 1;
  }
  if (unlockedTipsCount / 10 != 0) {
    Renderer2D::DrawSprite(Digits[unlockedTipsCount / 10], TIPSCountPositions[0]);
  }
  Renderer2D::DrawSprite(Digits[unlockedTipsCount % 10], TIPSCountPositions[1]);
}

inline void ClearListMenu::DrawAlbumCompletion() {
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
    Renderer2D::DrawSprite(Digits[percentage / 100], AlbumPositions[0]);
    Renderer2D::DrawSprite(Digits[(percentage / 10) % 10], AlbumPositions[1]);
  } else if (percentage / 10 != 0) {
    Renderer2D::DrawSprite(Digits[(percentage / 10) % 10], AlbumPositions[1]);
  }
  Renderer2D::DrawSprite(Digits[percentage % 10], AlbumPositions[2]);
}

inline void ClearListMenu::DrawEndingTree() {
  for (int i = 0; i < 8; i++) {
    Renderer2D::DrawSprite(EndingBox, BoxPositions[i]);
    // Flag for the 1st ending, they are contiguous
    if (GetFlag(SF_CLR_END1 + i)) {
      Renderer2D::DrawSprite(EndingThumbnails[i], ThumbnailPositions[i]);
    } else {
      Renderer2D::DrawSprite(LockedThumbnail, ThumbnailPositions[i]);
    }
  }
  Renderer2D::DrawSprite(EndingList, ListPosition);
}

inline void ClearListMenu::DrawRedBar() {
  if (RedBarKickIn.IsIn() && !RedBarAnimation.IsIn()) {
    RedBarAnimation.StartIn();
  }
  if (RedBarAnimation.IsIn()) {
    Renderer2D::DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (RedBarAnimation.State == AS_Playing) {
    float pixelPerAdvanceLeft = 935 * RedBarAnimation.Progress + 62;
    RedBarSprite.Bounds.X = 1825 - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = 1059 - pixelPerAdvanceLeft;
    Renderer2D::DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 195 * RedBarAnimation.Progress + 13;
    RedBarSprite.Bounds.X = 1826;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer2D::DrawSprite(RedBarSprite, RedBarPosition);
  }
}


}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto
