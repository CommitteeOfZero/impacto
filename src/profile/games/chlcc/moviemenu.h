#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MovieMenu {

int const Movies = 10;

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
extern Sprite RedBarLabel;
extern glm::vec2 MenuTitleTextRightPosition;
extern glm::vec2 MenuTitleTextLeftPosition;
extern float MenuTitleTextAngle;
extern float TitleFadeInDuration;
extern float TitleFadeOutDuration;
extern Sprite MenuTitleText;

extern Sprite SelectMovie[11];
extern glm::vec2 SelectMoviePos[11];
extern glm::vec2 LabelPosition;
extern Sprite MovieLabel;
extern Sprite MovieList;
extern glm::vec2 BoxPositions[Movies];
extern glm::vec2 ListPosition;
extern Sprite MovieBox;
extern glm::vec2 ThumbnailPositions[Movies];
extern Sprite MoviesThumbnails[Movies];
extern Sprite LockedThumbnail;
extern glm::vec2 ButtonPromptPosition;
extern Sprite ButtonPromptSprite;

void Configure();

}  // namespace MovieMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto