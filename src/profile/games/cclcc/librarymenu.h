#pragma once

#include "../../../spritesheet.h"
#include <enum.h>

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace LibraryMenu {

BETTER_ENUM(LibraryMenuPageType, int, Album, Sound, Movie)

inline Sprite LibraryBackgroundSprite;
inline glm::vec2 LibraryBackgroundPosition;
inline Sprite LibraryIndexSprite;
inline glm::vec2 LibraryIndexPosition;
inline Sprite LibraryButtonGuideSprite;
inline glm::vec2 LibraryButtonGuidePosition;
inline Sprite LibraryMaskSprite;

inline Sprite SnapPhotoSpriteHover;
inline Sprite SnapPhotoSpriteSelect;
inline glm::vec2 SnapPhotoPos;
inline Sprite HitSongsSpriteHover;
inline Sprite HitSongsSpriteSelect;
inline glm::vec2 HitSongsPos;
inline Sprite LoveMovieSpriteHover;
inline Sprite LoveMovieSpriteSelect;
inline glm::vec2 LoveMoviePos;

inline std::vector<Sprite> MovieDiskSprites;
inline std::vector<Sprite> MovieDiskHighlightSprites;
inline std::vector<glm::vec2> MovieDiskDisplayPositions;
inline std::vector<int> MovieDiskPlayIds;

inline float FadeInDuration;
inline float FadeOutDuration;
inline float SubMenuFadeInDuration;
inline float SubMenuFadeOutDuration;

void Configure();

}  // namespace LibraryMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto