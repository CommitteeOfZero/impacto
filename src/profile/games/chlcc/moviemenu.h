#pragma once

#include "../../../spritesheet.h"
#include "../../../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MovieMenu {

int constexpr Movies = 10;

inline float MenuTransitionDuration;
inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 CircleStartPosition;
inline float CircleOffset;
inline glm::vec2 ErinPosition;
inline Sprite ErinSprite;
inline Sprite BackgroundFilter;
inline glm::vec2 InitialRedBarPosition;
inline Sprite InitialRedBarSprite;
inline glm::vec2 RightRedBarPosition;
inline glm::vec2 RedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline Sprite RedBarSprite;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline Sprite MenuTitleText;
inline SpriteAnimationDef SelectedMovieAnimation;
inline Sprite SelectedMovieYellowDot;

inline float SelectMovieFadeDuration;
inline Sprite SelectMovie[11];
inline glm::vec2 SelectMoviePos[11];
inline glm::vec2 LabelPosition;
inline Sprite MovieLabel;
inline Sprite MovieList;
inline glm::vec2 BoxPositions[Movies];
inline glm::vec2 ListPosition;
inline Sprite MovieBox;
inline glm::vec2 ThumbnailPositions[Movies];
inline Sprite MoviesThumbnails[Movies];
inline Sprite LockedThumbnail;
inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;

inline bool MovieExtraVideosEnabled;
inline SpriteAnimationDef SelectedMovieExtraAnimation;
inline Sprite MovieBoxExtra;
inline Sprite MovieThumbnailExtraOp;
inline glm::vec2 MovieButtonExtraPromptPosition;
inline Sprite MovieButtonExtraPrompt;

void Configure();

}  // namespace MovieMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto