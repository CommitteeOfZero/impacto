#include "albummenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/albummenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace AlbumMenu {

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
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  CGList = EnsureGetMember<Sprite>("CGList");
  CGListPosition = EnsureGetMember<glm::vec2>("CGListPosition");
  PageCountLabel = EnsureGetMember<Sprite>("PageCountLabel");
  PageLabelPosition = EnsureGetMember<glm::vec2>("PageLabelPosition");
  CGBox = EnsureGetMember<Sprite>("CGBox");
  CGBoxTemplatePosition = EnsureGetMember<glm::vec2>("CGBoxTemplatePosition");
  GetMemberArray<Sprite>(AlbumThumbnails, 63, "AlbumThumbnails");
  ThumbnailTemplatePosition =
      EnsureGetMember<glm::vec2>("ThumbnailTemplatePosition");
  VariationUnlocked = EnsureGetMember<Sprite>("VariationUnlocked");
  VariationLocked = EnsureGetMember<Sprite>("VariationLocked");
  VariationTemplateOffset =
      EnsureGetMember<glm::vec2>("VariationTemplateOffset");
  LockedCG = EnsureGetMember<Sprite>("LockedCG");
  ThumbnailOffset = EnsureGetMember<glm::vec2>("ThumbnailOffset");
  ThumbnailHighlight = EnsureGetMember<Sprite>("ThumbnailHighlight");
  AlbumPages = EnsureGetMember<int>("AlbumPages");
  EntriesPerPage = EnsureGetMember<int>("EntriesPerPage");
  GetMemberArray<Sprite>(PageNums, 10, "PageNums");
  CurrentPageNumPos = EnsureGetMember<glm::vec2>("CurrentPageNumPos");
  MaxPageNumPos = EnsureGetMember<glm::vec2>("MaxPageNumPos");
  PageNumSeparatorSlash = EnsureGetMember<Sprite>("PageNumSeparatorSlash");
  PageNumSeparatorSlashPos =
      EnsureGetMember<glm::vec2>("PageNumSeparatorSlashPos");
  GetMemberArray<Sprite>(ReachablePageNums, 10, "ReachablePageNums");
  ButtonGuide = EnsureGetMember<Sprite>("ButtonGuide");
  ButtonGuidePos = EnsureGetMember<glm::vec2>("ButtonGuidePos");
  GetMemberArray<Sprite>(SelectData, 10, "SelectData");
  GetMemberArray<glm::vec2>(SelectDataPos, 10, "SelectDataPos");
  AlbumMenuTitle = EnsureGetMember<Sprite>("AlbumMenuTitle");
  AlbumMenuTitleRightPos = EnsureGetMember<glm::vec2>("AlbumMenuTitleRightPos");
  AlbumMenuTitleLeftPos = EnsureGetMember<glm::vec2>("AlbumMenuTitleLeftPos");
  AlbumMenuTitleAngle = EnsureGetMember<float>("AlbumMenuTitleAngle");
  CgViewerButtonGuideVariation =
      EnsureGetMember<Sprite>("CgViewerButtonGuideVariation");
  CgViewerButtonGuideNoVariation =
      EnsureGetMember<Sprite>("CgViewerButtonGuideNoVariation");
  CgViewerButtonGuidePos = EnsureGetMember<glm::vec2>("CgViewerButtonGuidePos");
  SelectionMarkerSprite = EnsureGetMember<Sprite>("SelectionMarkerSprite");
  SelectionMarkerRelativePos =
      EnsureGetMember<glm::vec2>("SelectionMarkerRelativePos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(new UI::CHLCC::AlbumMenu());
}

}  // namespace AlbumMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto