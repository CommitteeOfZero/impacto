#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MusicMenu {

int constexpr Endings = 8;
int constexpr MusicTrackCount = 45;

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
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline Sprite TrackTree;
inline glm::vec2 TrackTreePos;
inline glm::vec2 TrackButtonPosTemplate;
inline glm::vec2 TrackNameOffset;
inline glm::vec2 ArtistOffset;
inline glm::vec2 TrackOffset;
inline Sprite TrackHighlight;
inline glm::vec2 TrackNumRelativePos;
inline glm::vec2 PlaymodeRepeatPos;
inline glm::vec2 PlaymodeAllPos;
inline Sprite PlaymodeRepeat;
inline Sprite PlaymodeAll;
inline Sprite PlaymodeRepeatHighlight;
inline Sprite PlaymodeAllHighlight;
inline float MusicDirectionalHoldTime;
inline float MusicDirectionalFocusTimeInterval;
inline Sprite NowPlaying;
inline glm::vec2 NowPlayingPos;
inline float NowPlayingAnimationDuration;
inline glm::vec2 PlayingTrackOffset;
inline glm::vec2 PlayingTrackArtistOffset;
inline Sprite SoundLibraryTitle;
inline glm::vec2 SoundLibraryTitleLeftPos;
inline glm::vec2 SoundLibraryTitleRightPos;
inline float SoundLibraryTitleAngle;
inline Sprite HighlightStar;
inline glm::vec2 HighlightStarRelativePos;
inline int Playlist[MusicTrackCount];
inline Sprite SelectSound[11];
inline glm::vec2 SelectSoundPos[11];
inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;

void Configure();

}  // namespace MusicMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto