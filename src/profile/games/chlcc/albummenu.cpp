#include "albummenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/albummenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace AlbumMenu {

float MenuTransitionDuration;
uint32_t BackgroundColor;
Sprite CircleSprite;
glm::vec2 CircleStartPosition;
float CircleOffset;
glm::vec2 ErinPosition;
Sprite ErinSprite;
Sprite BackgroundFilter;
glm::vec2 InitialRedBarPosition;
glm::vec2 RightRedBarPosition;
Sprite InitialRedBarSprite;
glm::vec2 RedBarPosition;
float RedBarDivision;
float RedBarBaseX;
Sprite RedBarSprite;
glm::vec2 RedBarLabelPosition;
Sprite RedBarLabel;
float TitleFadeInDuration;
float TitleFadeOutDuration;
Sprite CGList;
glm::vec2 CGListPosition;
Sprite PageCountLabel;
glm::vec2 PageLabelPosition;
Sprite CGBox;
glm::vec2 CGBoxTemplatePosition;
Sprite AlbumThumbnails[63];
glm::vec2 ThumbnailTemplatePosition;
Sprite VariationUnlocked;
Sprite VariationLocked;
glm::vec2 VariationTemplateOffset;
Sprite LockedCG;
glm::vec2 ThumbnailOffset;
Sprite ThumbnailHighlight;
int32_t AlbumPages;
int32_t EntriesPerPage;
Sprite PageNums[10];
glm::vec2 CurrentPageNumPos;
glm::vec2 MaxPageNumPos;
Sprite PageNumSeparatorSlash;
glm::vec2 PageNumSeparatorSlashPos;
Sprite ReachablePageNums[10];
Sprite ButtonGuide;
glm::vec2 ButtonGuidePos;
Sprite SelectData[10];
glm::vec2 SelectDataPos[10];
Sprite AlbumMenuTitle;
glm::vec2 AlbumMenuTitleRightPos;
glm::vec2 AlbumMenuTitleLeftPos;
float AlbumMenuTitleAngle;
Sprite CgViewerButtonGuideVariation;
Sprite CgViewerButtonGuideNoVariation;
glm::vec2 CgViewerButtonGuidePos;
Sprite SelectionMarkerSprite;
glm::vec2 SelectionMarkerRelativePos;

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
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  CGList = EnsureGetMemberSprite("CGList");
  CGListPosition = EnsureGetMemberVec2("CGListPosition");
  PageCountLabel = EnsureGetMemberSprite("PageCountLabel");
  PageLabelPosition = EnsureGetMemberVec2("PageLabelPosition");
  CGBox = EnsureGetMemberSprite("CGBox");
  CGBoxTemplatePosition = EnsureGetMemberVec2("CGBoxTemplatePosition");
  GetMemberSpriteArray(AlbumThumbnails, 63, "AlbumThumbnails");
  ThumbnailTemplatePosition = EnsureGetMemberVec2("ThumbnailTemplatePosition");
  VariationUnlocked = EnsureGetMemberSprite("VariationUnlocked");
  VariationLocked = EnsureGetMemberSprite("VariationLocked");
  VariationTemplateOffset = EnsureGetMemberVec2("VariationTemplateOffset");
  LockedCG = EnsureGetMemberSprite("LockedCG");
  ThumbnailOffset = EnsureGetMemberVec2("ThumbnailOffset");
  ThumbnailHighlight = EnsureGetMemberSprite("ThumbnailHighlight");
  AlbumPages = EnsureGetMemberInt("AlbumPages");
  EntriesPerPage = EnsureGetMemberInt("EntriesPerPage");
  GetMemberSpriteArray(PageNums, 10, "PageNums");
  CurrentPageNumPos = EnsureGetMemberVec2("CurrentPageNumPos");
  MaxPageNumPos = EnsureGetMemberVec2("MaxPageNumPos");
  PageNumSeparatorSlash = EnsureGetMemberSprite("PageNumSeparatorSlash");
  PageNumSeparatorSlashPos = EnsureGetMemberVec2("PageNumSeparatorSlashPos");
  GetMemberSpriteArray(ReachablePageNums, 10, "ReachablePageNums");
  ButtonGuide = EnsureGetMemberSprite("ButtonGuide");
  ButtonGuidePos = EnsureGetMemberVec2("ButtonGuidePos");
  GetMemberSpriteArray(SelectData, 10, "SelectData");
  GetMemberVec2Array(SelectDataPos, 10, "SelectDataPos");
  AlbumMenuTitle = EnsureGetMemberSprite("AlbumMenuTitle");
  AlbumMenuTitleRightPos = EnsureGetMemberVec2("AlbumMenuTitleRightPos");
  AlbumMenuTitleLeftPos = EnsureGetMemberVec2("AlbumMenuTitleLeftPos");
  AlbumMenuTitleAngle = EnsureGetMemberFloat("AlbumMenuTitleAngle");
  CgViewerButtonGuideVariation =
      EnsureGetMemberSprite("CgViewerButtonGuideVariation");
  CgViewerButtonGuideNoVariation =
      EnsureGetMemberSprite("CgViewerButtonGuideNoVariation");
  CgViewerButtonGuidePos = EnsureGetMemberVec2("CgViewerButtonGuidePos");
  SelectionMarkerSprite = EnsureGetMemberSprite("SelectionMarkerSprite");
  SelectionMarkerRelativePos =
      EnsureGetMemberVec2("SelectionMarkerRelativePos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::CHLCC::AlbumMenu());
}

}  // namespace AlbumMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto