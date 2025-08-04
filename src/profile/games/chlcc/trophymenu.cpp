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
  MenuTransitionDuration = EnsureGetMember<float>("TransitionDuration");
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");
  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPromptSprite");
  DefaultTrophyIconSprite = EnsureGetMember<Sprite>("DefaultTrophyIconSprite");
  TrophyEntryCardSprite = EnsureGetMember<Sprite>("TrophyEntryCardSprite");
  PlatinumTrophySprite = EnsureGetMember<Sprite>("PlatinumTrophySprite");
  PlatinumTrophyPos = EnsureGetMember<glm::vec2>("PlatinumTrophyPos");
  GoldTrophySprite = EnsureGetMember<Sprite>("GoldTrophySprite");
  GoldTrophyPos = EnsureGetMember<glm::vec2>("GoldTrophyPos");
  SilverTrophySprite = EnsureGetMember<Sprite>("SilverTrophySprite");
  SilverTrophyPos = EnsureGetMember<glm::vec2>("SilverTrophyPos");
  BronzeTrophySprite = EnsureGetMember<Sprite>("BronzeTrophySprite");
  BronzeTrophyPos = EnsureGetMember<glm::vec2>("BronzeTrophyPos");
  TrophyEntriesBorderSprite =
      EnsureGetMember<Sprite>("TrophyEntriesBorderSprite");
  TrophyPageCtBoxSprite = EnsureGetMember<Sprite>("TrophyPageCtBoxSprite");
  GetMemberArray<Sprite>(PageNums, 10, "PageNums");
  GetMemberArray<Sprite>(ReachablePageNums, 10, "ReachablePageNums");
  PageNumSeparatorSlash =
      EnsureGetMember<Sprite>("PageNumSeparatorSlashSprite");
  CurrentPageNumPos = EnsureGetMember<glm::vec2>("CurrentPageNumPos");
  PageNumSeparatorPos = EnsureGetMember<glm::vec2>("PageNumSeparatorSlashPos");
  MaxPageNumPos = EnsureGetMember<glm::vec2>("MaxPageNumPos");
  TrophyCountHintLabelPos =
      EnsureGetMember<glm::vec2>("TrophyCountHintLabelPos");
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::TrophyMenuPtr = new UI::CHLCC::TrophyMenu();
  UI::Menus[drawType].push_back(UI::TrophyMenuPtr);
}

}  // namespace TrophyMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
