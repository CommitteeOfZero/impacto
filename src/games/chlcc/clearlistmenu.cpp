#include "clearlistmenu.h"

#include "../../profile/games/chlcc/clearlistmenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"
#include "../../data/tipssystem.h"
#include "../../profile/games/chlcc/commonmenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::CommonMenu;
using namespace Impacto::Profile::CHLCC::ClearListMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::GameSpecific;
using namespace Impacto::UI::Widgets;
using namespace Impacto::TipsSystem;
using namespace Impacto::Profile;

ClearListMenu::ClearListMenu() : CommonMenu(false) {}

void ClearListMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
    }
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
  if (State == Hidden) return;
  CommonMenu::DrawSubmenu(BackgroundColor, CircleSprite, MenuTitleText,
                          MenuTitleTextAngle, true);

  if (MenuTransition.Progress < 0.22f) return;
  float yOffset = MenuTransition.GetPageOffset().y;

  Renderer->DrawSprite(ClearListLabel,
                       glm::vec2(LabelPosition.x, LabelPosition.y + yOffset));
  DrawPlayTime(yOffset);
  DrawEndingCount(yOffset);
  DrawTIPSCount(yOffset);
  DrawAlbumCompletion(yOffset);
  DrawEndingTree(yOffset);
  CommonMenu::DrawButtonPrompt(ButtonPromptSprite, ButtonPromptPosition);
}

void ClearListMenu::Update(float dt) {
  if ((!GetFlag(SF_CLEARLISTMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_CLEARLISTMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() &&
      (ScrWork[SW_SYSMENUCT] == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
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
    UpdateTitles(MenuTitleTextRightPosition, MenuTitleTextLeftPosition);
  }
}

inline void ClearListMenu::DrawPlayTime(float yOffset) {
  int totalSeconds = ScrWork[SW_TOTALPLAYTIME];
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
    if (!(idx == 0 && time[idx] == 0)) {
      glm::vec2 position(TimePositions[idx].x, TimePositions[idx].y + yOffset);
      Renderer->DrawSprite(Digits[time[idx]], position);
    }
  }
}

inline void ClearListMenu::DrawEndingCount(float yOffset) {
  int unlockedEndingCount = 0;
  for (int i = 0; i < Endings; i++) {
    unlockedEndingCount += GetFlag(SF_CLR_END1 + i);
  }
  glm::vec2 position(EndingCountPosition.x, EndingCountPosition.y + yOffset);
  Renderer->DrawSprite(Digits[unlockedEndingCount], position);
}

inline void ClearListMenu::DrawTIPSCount(float yOffset) {
  int unlockedTipsCount = 0;
  size_t totalTips = GetTipCount();

  for (size_t idx = 0; idx < totalTips; idx++) {
    unlockedTipsCount += GetTipLockedState(idx) ? 0 : 1;
  }

  const int hundreds = unlockedTipsCount / 100;
  const int tens = (unlockedTipsCount / 10) % 10;
  const int ones = unlockedTipsCount % 10;

  if (hundreds > 0) {
    Renderer->DrawSprite(
        Digits[hundreds],
        glm::vec2(TIPSCountPositions[0].x, TIPSCountPositions[0].y + yOffset));
  }
  if (hundreds > 0 || tens > 0) {
    Renderer->DrawSprite(
        Digits[tens],
        glm::vec2(TIPSCountPositions[1].x, TIPSCountPositions[1].y + yOffset));
  }
  Renderer->DrawSprite(
      Digits[ones],
      glm::vec2(TIPSCountPositions[2].x, TIPSCountPositions[2].y + yOffset));
}

inline void ClearListMenu::DrawAlbumCompletion(float yOffset) {
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
    Renderer->DrawSprite(
        Digits[percentage / 100],
        glm::vec2(AlbumPositions[0].x, AlbumPositions[0].y + yOffset));
    Renderer->DrawSprite(
        Digits[(percentage / 10) % 10],
        glm::vec2(AlbumPositions[1].x, AlbumPositions[1].y + yOffset));
  } else if (percentage / 10 != 0) {
    Renderer->DrawSprite(
        Digits[(percentage / 10) % 10],
        glm::vec2(AlbumPositions[1].x, AlbumPositions[1].y + yOffset));
  }
  Renderer->DrawSprite(
      Digits[percentage % 10],
      glm::vec2(AlbumPositions[2].x, AlbumPositions[2].y + yOffset));
}

inline void ClearListMenu::DrawEndingTree(float yOffset) {
  for (int i = 0; i < Endings; i++) {
    glm::vec2 boxPosition(BoxPositions[i].x, BoxPositions[i].y + yOffset);
    glm::vec2 thumbnailPosition(ThumbnailPositions[i].x,
                                ThumbnailPositions[i].y + yOffset);
    Renderer->DrawSprite(EndingBox, boxPosition);
    // Flag for the 1st ending, they are contiguous
    if (GetFlag(SF_CLR_END1 + i)) {
      Renderer->DrawSprite(EndingThumbnails[i], thumbnailPosition);
    } else {
      Renderer->DrawSprite(LockedThumbnail, thumbnailPosition);
    }
  }
  glm::vec2 listPosition(ListPosition.x, ListPosition.y + yOffset);
  Renderer->DrawSprite(EndingList, listPosition);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto
