#include "albummenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/albummenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace AlbumMenu {

void Configure() {
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CGList = EnsureGetMember<Sprite>("CGList");
  CGListPosition = EnsureGetMember<glm::vec2>("CGListPosition");
  PageCountLabel = EnsureGetMember<Sprite>("PageCountLabel");
  PageLabelPosition = EnsureGetMember<glm::vec2>("PageLabelPosition");
  CGBox = EnsureGetMember<Sprite>("CGBox");
  CGBoxTemplatePosition = EnsureGetMember<glm::vec2>("CGBoxTemplatePosition");
  GetMemberArray<Sprite>(std::span(AlbumThumbnails, 63), "AlbumThumbnails");
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
  GetMemberArray<Sprite>(std::span(PageNums, 10), "PageNums");
  CurrentPageNumPos = EnsureGetMember<glm::vec2>("CurrentPageNumPos");
  MaxPageNumPos = EnsureGetMember<glm::vec2>("MaxPageNumPos");
  PageNumSeparatorSlash = EnsureGetMember<Sprite>("PageNumSeparatorSlash");
  PageNumSeparatorSlashPos =
      EnsureGetMember<glm::vec2>("PageNumSeparatorSlashPos");
  GetMemberArray<Sprite>(std::span(ReachablePageNums, 10), "ReachablePageNums");
  ButtonGuide = EnsureGetMember<Sprite>("ButtonGuide");
  ButtonGuidePos = EnsureGetMember<glm::vec2>("ButtonGuidePos");
  SelectDataSprites = EnsureGetMember<std::vector<Sprite>>("SelectDataSprites");
  SelectDataPos = EnsureGetMember<std::vector<glm::vec2>>("SelectDataPos");
  if (SelectDataSprites.size() != SelectDataPos.size()) {
    throw std::runtime_error("Related arrays have mismatching sizes");
  }
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
  CgFadeDuration = EnsureGetMember<float>("CgFadeDuration");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::Menus[drawType].push_back(new UI::CHLCC::AlbumMenu());
}

}  // namespace AlbumMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto