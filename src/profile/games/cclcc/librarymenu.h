#pragma once

#include "../../../spritesheet.h"
#include <enum.h>

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace LibraryMenu {

struct AlbumDataEntry {
  std::vector<Sprite> ThumbnailSprites;
  glm::vec2 Position;
  uint8_t IndexInPage;
  uint8_t PageNumber;
};

BETTER_ENUM(LibraryMenuPageType, int, Album, Sound, Movie)
BETTER_ENUM(MusicMenuPlayingMode, int, RepeatOne, PlayAll, RepeatAll, Shuffle)

inline Sprite LibraryBackgroundSprite;
inline glm::vec2 LibraryBackgroundPosition;
inline Sprite LibraryIndexSprite;
inline glm::vec2 LibraryIndexPosition;
inline glm::vec2 LibraryButtonGuidePosition;
inline Sprite LibraryMaskSprite;
inline float LibraryTransitionPositionOffset;

inline Sprite SnapPhotoSpriteHover;
inline Sprite SnapPhotoSpriteSelect;
inline glm::vec2 SnapPhotoPos;
inline Sprite HitSongsSpriteHover;
inline Sprite HitSongsSpriteSelect;
inline glm::vec2 HitSongsPos;
inline Sprite LoveMovieSpriteHover;
inline Sprite LoveMovieSpriteSelect;
inline glm::vec2 LoveMoviePos;

inline std::vector<glm::vec2> AlbumThumbDispPos;
inline std::vector<AlbumDataEntry> AlbumData;
inline std::vector<Sprite> AlbumThumbnailPinSprites;
inline Sprite AlbumThumbnailThumbSprite;
inline float AlbumPageSwapAnimationDuration;
inline glm::vec2 AlbumThumbnailPinRemoveOffset;

inline int MusicItemsBackgroundRepeatHeight;
inline Sprite MusicItemsBackgroundSprite;
inline glm::vec2 MusicItemsBackgroundPosition;
inline Sprite MusicItemsOverlaySprite;
inline glm::vec2 MusicItemsOverlayPosition;
inline std::vector<uint8_t> MusicPlayIds;
inline std::vector<uint8_t> MusicBGMFlagIds;
inline RectF MusicRenderingBounds;
inline RectF MusicHoverBounds;
inline uint8_t MusicStringTableId;
inline int MusicStringLockedIndex;
inline float MusicTrackNameSize;
inline int MusicTrackNameOffsetX;
inline int MusicTrackArtistSize;
inline int MusicTrackArtistOffsetX;
inline int MusicTrackNumberOffsetX;
inline uint32_t MusicButtonTextColor;
inline uint32_t MusicButtonTextOutlineColor;
inline int MusicButtonTextYOffset;
inline glm::vec2 MusicButtonPlayingDispOffset;
inline float MusicDirectionalHoldTime;
inline float MusicDirectionalFocusTimeInterval;
inline RectF MusicButtonBounds;
inline Sprite MusicButtonHoverSprite;
inline Sprite MusicButtonSelectSprite;
inline Sprite MusicButtonPlayingSprite;
inline Sprite MusicNowPlayingNotificationSprite;
inline glm::vec2 MusicNowPlayingNotificationPos;
inline float MusicNowPlayingNotificationFadeIn;
inline float MusicNowPlayingNotificationFadeOut;
inline glm::vec2 MusicNowPlayingNotificationTrackOffset;
inline int MusicNowPlayingNotificationTrackFontSize;
inline uint32_t MusicNowPlayingTextColor;
inline uint32_t MusicNowPlayingTextOutlineColor;
inline std::array<Sprite, MusicMenuPlayingMode::_size()>
    MusicPlayingModeSprites;
inline std::array<RectF, MusicMenuPlayingMode::_size()>
    MusicPlayingModeDisplayBounds;

inline std::vector<Sprite> MovieDiskSprites;
inline std::vector<Sprite> MovieDiskHighlightSprites;
inline std::vector<glm::vec2> MovieDiskDisplayPositions;
inline std::vector<int> MovieDiskPlayIds;

inline Sprite AlbumMenuGuideSprite;
inline Sprite AlbumMenuCGViewerGuideSprite;
inline Sprite MusicMenuGuideSprite;
inline Sprite MovieMenuGuideSprite;

inline float FadeInDuration;
inline float FadeOutDuration;
inline float SubMenuFadeInDuration;
inline float SubMenuFadeOutDuration;
inline float ButtonBlinkDuration;
inline float ButtonBlinkTintMinimum;

void Configure();

}  // namespace LibraryMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto