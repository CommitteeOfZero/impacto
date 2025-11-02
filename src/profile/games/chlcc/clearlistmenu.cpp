#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/clearlistmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace ClearListMenu {

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

  LabelPosition = EnsureGetMember<glm::vec2>("LabelPosition");
  ClearListLabel = EnsureGetMember<Sprite>("ClearListLabel");
  GetMemberArray<glm::vec2>(TimePositions, 6, "TimePositions");
  EndingCountPosition = EnsureGetMember<glm::vec2>("EndingCountPosition");
  GetMemberArray<glm::vec2>(TIPSCountPositions, 3, "TIPSCountPositions");
  GetMemberArray<Sprite>(Digits, 10, "Digits");
  GetMemberArray<glm::vec2>(AlbumPositions, 3, "AlbumPositions");
  ListPosition = EnsureGetMember<glm::vec2>("ListPosition");
  EndingList = EnsureGetMember<Sprite>("EndingList");
  GetMemberArray<glm::vec2>(BoxPositions, Endings, "BoxPositions");
  EndingBox = EnsureGetMember<Sprite>("EndingBox");
  GetMemberArray<glm::vec2>(ThumbnailPositions, Endings, "ThumbnailPositions");
  GetMemberArray<Sprite>(EndingThumbnails, Endings, "EndingThumbnails");
  LockedThumbnail = EnsureGetMember<Sprite>("LockedThumbnail");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPromptSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  auto clearList = new UI::CHLCC::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto