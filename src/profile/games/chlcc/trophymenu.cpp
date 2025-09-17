#include "trophymenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/trophymenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/trophymenu.h"
#include "../../../data/achievementsystem.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TrophyMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMember<float>("TransitionDuration");

  ShowPageAnimationStartTime =
      EnsureGetMember<float>("ShowPageAnimationStartTime");
  ShowPageAnimationDuration =
      EnsureGetMember<float>("ShowPageAnimationDuration");

  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");

  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");

  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");

  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
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

  PlatinumTrophySprite = EnsureGetMember<Sprite>("PlatinumTrophySprite");
  PlatinumTrophyPos = EnsureGetMember<glm::vec2>("PlatinumTrophyPos");
  GoldTrophySprite = EnsureGetMember<Sprite>("GoldTrophySprite");
  GoldTrophyPos = EnsureGetMember<glm::vec2>("GoldTrophyPos");
  SilverTrophySprite = EnsureGetMember<Sprite>("SilverTrophySprite");
  SilverTrophyPos = EnsureGetMember<glm::vec2>("SilverTrophyPos");
  BronzeTrophySprite = EnsureGetMember<Sprite>("BronzeTrophySprite");
  BronzeTrophyPos = EnsureGetMember<glm::vec2>("BronzeTrophyPos");

  DefaultTrophyIconSprite = EnsureGetMember<Sprite>("DefaultTrophyIconSprite");
  TrophyEntryCardSprite = EnsureGetMember<Sprite>("TrophyEntryCardSprite");
  TrophyEntriesBorderSprite =
      EnsureGetMember<Sprite>("TrophyEntriesBorderSprite");
  TrophyPageCtBoxSprite = EnsureGetMember<Sprite>("TrophyPageCtBoxSprite");
  TrophyPageCtPos = EnsureGetMember<glm::vec2>("TrophyPageCtPos");

  EntriesPerPage = EnsureGetMember<size_t>("EntriesPerPage");
  FirstEntryPos = EnsureGetMember<glm::vec2>("FirstEntryPos");
  EntryHeight = EnsureGetMember<float>("EntryHeight");
  EntryCardOffset = EnsureGetMember<glm::vec2>("EntryCardOffset");
  EntryNameOffset = EnsureGetMember<glm::vec2>("EntryNameOffset");
  EntryNameFontSize = EnsureGetMember<float>("EntryNameFontSize");
  EntryDescriptionOffset = EnsureGetMember<glm::vec2>("EntryDescriptionOffset");
  EntryDescriptionFontSize = EnsureGetMember<float>("EntryDescriptionFontSize");
  EntryIconOffset = EnsureGetMember<glm::vec2>("EntryIconOffset");
  EntryDefaultNameTextTableId =
      EnsureGetMember<uint32_t>("EntryDefaultNameTextTableId");
  EntryDefaultNameStringNum =
      EnsureGetMember<uint32_t>("EntryDefaultNameStringNum");

  CurrentPageNumPos = EnsureGetMember<glm::vec2>("CurrentPageNumPos");
  PageNumSeparatorSlash =
      EnsureGetMember<Sprite>("PageNumSeparatorSlashSprite");
  PageNumSeparatorPos = EnsureGetMember<glm::vec2>("PageNumSeparatorSlashPos");
  MaxPageNumPos = EnsureGetMember<glm::vec2>("MaxPageNumPos");
  GetMemberArray<Sprite>(PageNums.data(), std::ssize(PageNums), "PageNums");
  GetMemberArray<Sprite>(ReachablePageNums.data(),
                         std::ssize(ReachablePageNums), "ReachablePageNums");

  TrophyCountHintTextTableId =
      EnsureGetMember<uint32_t>("TrophyCountHintTextTableId");
  TrophyCountHintStringNum =
      EnsureGetMember<uint32_t>("TrophyCountHintStringNum");
  TrophyCountHintLabelPos =
      EnsureGetMember<glm::vec2>("TrophyCountHintLabelPos");
  TrophyCountFontSize = EnsureGetMember<float>("TrophyCountFontSize");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  assert(MaxTrophyPages * EntriesPerPage >=
         AchievementSystem::GetAchievementCount());

  UI::TrophyMenuPtr = new UI::CHLCC::TrophyMenu();
  UI::Menus[drawType].push_back(UI::TrophyMenuPtr);
}

}  // namespace TrophyMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
