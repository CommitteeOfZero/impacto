#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MusicMenu {

int constexpr MusicTrackCount = 39;
int constexpr MusicPlaybackModeLabelCount = 4;
int constexpr TimerCharCount = 11;

void Configure();

inline Sprite BackgroundSprite;

inline Sprite Thumbnails[MusicTrackCount];
inline glm::vec2 ThumbnailPosition;
inline Sprite ItemsWindow;
inline glm::vec2 ItemsWindowPosition;
inline RectF ItemsWindowRenderingBounds;
inline glm::vec2 MusicListMargin;
inline glm::vec2 MusicListInitialPosition;
inline Sprite PlaybackWindow;
inline glm::vec2 PlaybackWindowPosition;
inline Sprite PlaybackModeLabels[MusicPlaybackModeLabelCount];
inline glm::vec2 PlaybackModeLabelPosition;
inline glm::vec2 CurrentlyPlayingLabelPosition;
inline Sprite ItemNames[MusicTrackCount];
inline glm::vec2 ItemNameHighlightOffset;
inline Sprite LockedItem;
inline Sprite ScrollbarThumb;
inline Sprite ScrollbarTrack;
inline glm::vec2 ScrollbarPosition;
inline float ScrollbarStart;
inline int Playlist[MusicTrackCount];
inline Sprite TimerChars[TimerCharCount];
inline glm::vec2 TimerInitialPosition;
inline glm::vec2 TimerMargin;

inline float FadeInDuration;
inline float FadeOutDuration;

}  // namespace MusicMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto