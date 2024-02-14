#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/clearlistmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace ClearListMenu {

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
glm::vec2 MenuTitleTextRightPosition;
glm::vec2 MenuTitleTextLeftPosition;
float MenuTitleTextAngle;
float TitleFadeInDuration;
float TitleFadeOutDuration;
Sprite MenuTitleText;

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
Sprite ButtonPromptSprite;

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
  ButtonPromptSprite = EnsureGetMemberSprite("ButtonPromptSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  auto clearList = new UI::CHLCC::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto