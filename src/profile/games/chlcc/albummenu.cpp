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
Sprite RedBarLabel; /*
glm::vec2 MenuTitleTextRightPosition;
glm::vec2 MenuTitleTextLeftPosition;
float MenuTitleTextAngle;*/
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

/*Sprite MenuTitleText;

glm::vec2 LabelPosition;
Sprite ClearListLabel;
glm::vec2 TimePositions[6];
glm::vec2 EndingCountPosition;
glm::vec2 TIPSCountPositions[2];
glm::vec2 AlbumPositions[3];
Sprite Digits[10];
glm::vec2 ListPosition;
Sprite EndingList;
glm::vec2 BoxPositions[Endings];
Sprite EndingBox;
glm::vec2 ThumbnailPositions[Endings];
Sprite EndingThumbnails[Endings];
Sprite LockedThumbnail;
glm::vec2 ButtonPromptPosition;
Sprite ButtonPromptSprite;*/

static void GetMemberVec2Array(glm::vec2* arr, uint32_t count,
                               char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d vec2 for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementVec2(i);
  }

  Pop();
}

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

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
  /*MenuTitleTextRightPosition = EnsureGetMemberVec2("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition = EnsureGetMemberVec2("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMemberFloat("MenuTitleTextAngle");*/
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

  /* MenuTitleText = EnsureGetMemberSprite("MenuTitleText");

   LabelPosition = EnsureGetMemberVec2("LabelPosition");
   ClearListLabel = EnsureGetMemberSprite("ClearListLabel");
   GetMemberVec2Array(TimePositions, 6, "TimePositions");
   EndingCountPosition = EnsureGetMemberVec2("EndingCountPosition");
   GetMemberVec2Array(TIPSCountPositions, 2, "TIPSCountPositions");
   GetMemberSpriteArray(Digits, 10, "Digits");
   GetMemberVec2Array(AlbumPositions, 3, "AlbumPositions");
   ListPosition = EnsureGetMemberVec2("ListPosition");
   EndingList = EnsureGetMemberSprite("EndingList");
   GetMemberVec2Array(BoxPositions, Endings, "BoxPositions");
   EndingBox = EnsureGetMemberSprite("EndingBox");
   GetMemberVec2Array(ThumbnailPositions, Endings, "ThumbnailPositions");
   GetMemberSpriteArray(EndingThumbnails, Endings, "EndingThumbnails");
   LockedThumbnail = EnsureGetMemberSprite("LockedThumbnail");
   ButtonPromptPosition = EnsureGetMemberVec2("ButtonPromptPosition");
   ButtonPromptSprite = EnsureGetMemberSprite("ButtonPromptSprite");*/

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::CHLCC::AlbumMenu());
}

}  // namespace AlbumMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto