#include "trophymenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/trophymenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/trophymenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TrophyMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMemberFloat("TransitionDuration");
  BackgroundColor = EnsureGetMemberUint("BackgroundColor");
  CircleSprite = EnsureGetMemberSprite("CircleSprite");
  CircleStartPosition = EnsureGetMemberVec2("CircleStartPosition");
  CircleOffset = EnsureGetMemberFloat("CircleOffset");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMemberVec2("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMemberVec2("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMemberSprite("RedBarSprite");
  RedBarDivision = EnsureGetMemberFloat("RedBarDivision");
  RedBarBaseX = EnsureGetMemberFloat("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMemberVec2("RedBarLabelPosition");
  RedBarLabel = EnsureGetMemberSprite("RedBarLabel");
  MenuTitleTextRightPosition = EnsureGetMemberVec2("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition = EnsureGetMemberVec2("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMemberFloat("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMemberSprite("MenuTitleText");
  ButtonPromptPosition = EnsureGetMemberVec2("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMemberSprite("ButtonPromptSprite");
  DefaultTrophyIconSprite = EnsureGetMemberSprite("DefaultTrophyIconSprite");
  TrophyEntryCardSprite = EnsureGetMemberSprite("TrophyEntryCardSprite");
  PlatinumTrophySprite = EnsureGetMemberSprite("PlatinumTrophySprite");
  PlatinumTrophyPos = EnsureGetMemberVec2("PlatinumTrophyPos");
  GoldTrophySprite = EnsureGetMemberSprite("GoldTrophySprite");
  GoldTrophyPos = EnsureGetMemberVec2("GoldTrophyPos");
  SilverTrophySprite = EnsureGetMemberSprite("SilverTrophySprite");
  SilverTrophyPos = EnsureGetMemberVec2("SilverTrophyPos");
  BronzeTrophySprite = EnsureGetMemberSprite("BronzeTrophySprite");
  BronzeTrophyPos = EnsureGetMemberVec2("BronzeTrophyPos");
  TrophyEntriesBorderSprite =
      EnsureGetMemberSprite("TrophyEntriesBorderSprite");
  TrophyPageCtBoxSprite = EnsureGetMemberSprite("TrophyPageCtBoxSprite");
  GetMemberSpriteArray(PageNums, 10, "PageNums");
  GetMemberSpriteArray(ReachablePageNums, 10, "ReachablePageNums");
  PageNumSeparatorSlash = EnsureGetMemberSprite("PageNumSeparatorSlashSprite");
  CurrentPageNumPos = EnsureGetMemberVec2("CurrentPageNumPos");
  PageNumSeparatorPos = EnsureGetMemberVec2("PageNumSeparatorSlashPos");
  MaxPageNumPos = EnsureGetMemberVec2("MaxPageNumPos");
  TrophyCountHintLabelPos = EnsureGetMemberVec2("TrophyCountHintLabelPos");
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TrophyMenuPtr = new UI::CHLCC::TrophyMenu();
  UI::Menus[drawType].push_back(UI::TrophyMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
