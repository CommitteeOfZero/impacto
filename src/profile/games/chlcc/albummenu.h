#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace AlbumMenu {

int const Endings = 8;

extern float MenuTransitionDuration;
extern uint32_t BackgroundColor;
extern Sprite CircleSprite;
extern glm::vec2 CircleStartPosition;
extern float CircleOffset;
extern glm::vec2 ErinPosition;
extern Sprite ErinSprite;
extern Sprite BackgroundFilter;
extern glm::vec2 InitialRedBarPosition;
extern Sprite InitialRedBarSprite;
extern glm::vec2 RightRedBarPosition;
extern glm::vec2 RedBarPosition;
extern float RedBarDivision;
extern float RedBarBaseX;
extern Sprite RedBarSprite;
extern glm::vec2 RedBarLabelPosition;
extern Sprite RedBarLabel; /*
extern glm::vec2 MenuTitleTextRightPosition;
extern glm::vec2 MenuTitleTextLeftPosition;
extern float MenuTitleTextAngle;*/
extern float TitleFadeInDuration;
extern float TitleFadeOutDuration;
extern Sprite CGList;
extern glm::vec2 CGListPosition;
extern Sprite PageCountLabel;
extern glm::vec2 PageLabelPosition;
extern Sprite CGBox;
extern glm::vec2 CGBoxTemplatePosition;
extern Sprite AlbumThumbnails[63];
extern glm::vec2 ThumbnailTemplatePosition;
extern Sprite VariationUnlocked;
extern Sprite VariationLocked;
extern glm::vec2 VariationTemplateOffset;
extern Sprite LockedCG;
extern glm::vec2 ThumbnailOffset;
extern Sprite ThumbnailHighlight;
extern int32_t AlbumPages;
extern int32_t EntriesPerPage;
extern Sprite PageNums[10];
extern glm::vec2 CurrentPageNumPos;
extern glm::vec2 MaxPageNumPos;
extern Sprite PageNumSeparatorSlash;
extern glm::vec2 PageNumSeparatorSlashPos;
extern Sprite ReachablePageNums[10];
extern Sprite ButtonGuide;
extern glm::vec2 ButtonGuidePos;
extern Sprite SelectData[10];
extern glm::vec2 SelectDataPos[10];
extern Sprite AlbumMenuTitle;
extern glm::vec2 AlbumMenuTitleRightPos;
extern glm::vec2 AlbumMenuTitleLeftPos;
extern float AlbumMenuTitleAngle;
extern Sprite CgViewerButtonGuideVariation;
extern Sprite CgViewerButtonGuideNoVariation;
extern glm::vec2 CgViewerButtonGuidePos; /*
extern Sprite MenuTitleText;

extern glm::vec2 LabelPosition;
extern Sprite ClearListLabel;
extern glm::vec2 TimePositions[6];
extern glm::vec2 EndingCountPosition;
extern glm::vec2 TIPSCountPositions[2];
extern glm::vec2 AlbumPositions[3];
extern Sprite Digits[10];
extern glm::vec2 ListPosition;
extern Sprite EndingList;
extern glm::vec2 BoxPositions[Endings];
extern Sprite EndingBox;
extern glm::vec2 ThumbnailPositions[Endings];
extern Sprite EndingThumbnails[Endings];
extern Sprite LockedThumbnail;
extern glm::vec2 ButtonPromptPosition;
extern Sprite ButtonPromptSprite;*/

void Configure();

}  // namespace AlbumMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto