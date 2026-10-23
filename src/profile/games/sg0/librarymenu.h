#pragma once

#include <vector>

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace LibraryMenu {

int constexpr TabCount = 4;      // CLEAR LIST, CG, SOUND, MOVIE
int constexpr LibraryCount = 3;  // CG, SOUND, MOVIE
int constexpr GuideCount = 4;    // CG list, CG viewer, sound, movie

void Configure();

inline Sprite BackgroundSprite;
inline Sprite TabSprites[TabCount];
inline Sprite TabSelectedSprites[TabCount];
inline glm::vec2 TabPos;
inline float TabPitch;
inline Sprite TitleSprites[LibraryCount];
inline glm::vec2 TitlePos;
inline Sprite GuideSprites[GuideCount];
// Guides are right-aligned to this x, at this y.
inline float GuideRight;
inline float GuideY;

// CG library: one thumbnail per Profile::SaveSystem::AlbumEvData entry.
inline std::vector<Sprite> ThumbnailSprites;
inline Sprite NoDataSprite;
inline Sprite NoDataSelectedSprite;
inline Sprite SelectedFrameSprite;
inline Sprite SelectedGlowSprite;
inline glm::vec2 CgGridPos;
inline glm::vec2 CgCellPitch;
inline int CgColumns;
inline int CgRows;
inline glm::vec2 ThumbnailOffset;
inline Sprite ScrollThumbSprite;
inline float ScrollX;
inline float ScrollTop;
inline float ScrollBottom;
// Thumbnails without an EV flag, unlocked once all six endings are cleared.
inline std::vector<int> AllClearCgs;
inline std::vector<int> EndingFlags;
inline int CgLoadBuffer;
inline float CgZoomMax;
inline float CgScrollSpeed;

// Sound library: the player window and its track list.
inline Sprite PlayerSprite;
inline glm::vec2 PlayerPos;
inline Sprite ListHeaderSprite;
inline Sprite ListRowSprites[2];
inline Sprite ListSelectedSprite;
inline RectF ListBounds;
inline float ListRowHeight;
inline int ListVisibleRows;
inline float ListTrackX;
inline float ListTitleX;
inline float ListArtistX;
inline float ListFontSize;
inline glm::vec2 NowPlayingPos;
inline float NowPlayingFontSize;
inline Sprite RepeatModeSprites[4];
inline glm::vec2 RepeatModePos;
inline Sprite PlayIconSprite;
inline int TrackStringBase;
inline int LockedTrackString;
// Per track: bgm.mpk ids (loop, then no-loop variant or -1) and, for the
// vocal songs, the movie whose viewing also unlocks them (-1 for none).
inline std::vector<int> TrackBgmIds;
inline std::vector<int> TrackBgmAltIds;
inline std::vector<int> TrackMovieUnlocks;

// Movie library.
inline std::vector<Sprite> MovieThumbnailSprites;
inline std::vector<int> MovieIds;
inline glm::vec2 MovieGridPos;
inline glm::vec2 MovieCellPitch;
inline int MovieColumns;
inline float MovieThumbnailScale;
inline float MovieTitleOffsetY;
inline int MovieStringBase;
inline float MovieFontSize;
// Per movie: (script id, line id) pairs, the line read just before the
// movie plays; the movie is unlocked once any of them has been read.
inline std::vector<std::vector<int>> MovieUnlockLines;

inline uint32_t TextColor;
inline uint32_t LockedTextColor;
inline uint32_t SelectedTextColor;
inline int SystemScriptBuffer;
inline int CountMax;

}  // namespace LibraryMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
