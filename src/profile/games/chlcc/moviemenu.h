#pragma once

#include "../../../spritesheet.h"
#include "../../../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MovieMenu {

int constexpr Movies = 10;

inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline Sprite MenuTitleText;
inline SpriteAnimationDef SelectedMovieAnimation;
inline Sprite SelectedMovieYellowDot;

inline float SelectMovieFadeDuration;
inline std::vector<Sprite> SelectMovie;
inline std::vector<glm::vec2> SelectMoviePos;
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
inline Sprite MovieThumbnailExtraOp2;
inline glm::vec2 MovieButtonExtraPromptPosition;
inline Sprite MovieButtonExtraPrompt;

void Configure();

}  // namespace MovieMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto