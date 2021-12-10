#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MusicMenu {

int const MusicTrackCount = 39;
int const MusicPlaybackModeLabelCount = 4;
int const TimerCharCount = 11;

void Configure();

extern Sprite BackgroundSprite;

extern Sprite Thumbnails[MusicTrackCount];
extern glm::vec2 ThumbnailPosition;
extern Sprite ItemsWindow;
extern glm::vec2 ItemsWindowPosition;
extern RectF ItemsWindowRenderingBounds;
extern glm::vec2 MusicListMargin;
extern glm::vec2 MusicListInitialPosition;
extern Sprite PlaybackWindow;
extern glm::vec2 PlaybackWindowPosition;
extern Sprite PlaybackModeLabels[MusicPlaybackModeLabelCount];
extern glm::vec2 PlaybackModeLabelPosition;
extern glm::vec2 CurrentlyPlayingLabelPosition;
extern Sprite ItemNames[MusicTrackCount];
extern glm::vec2 ItemNameHighlightOffset;
extern Sprite LockedItem;
extern Sprite ScrollbarThumb;
extern Sprite ScrollbarTrack;
extern glm::vec2 ScrollbarPosition;
extern float ScrollbarStart;
extern int Playlist[MusicTrackCount];
extern Sprite TimerChars[TimerCharCount];
extern glm::vec2 TimerInitialPosition;
extern glm::vec2 TimerMargin;

extern float FadeInDuration;
extern float FadeOutDuration;

}  // namespace MusicMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto