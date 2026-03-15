#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/clearlistmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace ClearListMenu {

void Configure() {
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
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