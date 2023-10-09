#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MusicMenu {

int const Endings = 8;
int const MusicTrackCount = 45;

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
extern float TitleFadeInDuration;
extern float TitleFadeOutDuration;
extern Sprite TrackTree;
extern glm::vec2 TrackTreePos;
extern glm::vec2 TrackButtonPosTemplate;
extern glm::vec2 TrackNameOffset;
extern glm::vec2 ArtistOffset;
extern glm::vec2 TrackOffset;
extern Sprite TrackHighlight;
extern glm::vec2 TrackNumRelativePos;
extern glm::vec2 PlaymodeRepeatPos;
extern glm::vec2 PlaymodeAllPos;
extern Sprite PlaymodeRepeat;
extern Sprite PlaymodeAll;
extern Sprite PlaymodeRepeatHighlight;
extern Sprite PlaymodeAllHighlight;
extern Sprite NowPlaying;
extern glm::vec2 NowPlayingPos;
extern float NowPlayingAnimationDuration;
extern glm::vec2 PlayingTrackOffset;
extern glm::vec2 PlayingTrackArtistOffset;
extern Sprite SoundLibraryTitle;
extern glm::vec2 SoundLibraryTitleLeftPos;
extern glm::vec2 SoundLibraryTitleRightPos;
extern float SoundLibraryTitleAngle;
extern Sprite HighlightStar;
extern glm::vec2 HighlightStarRelativePos;
extern int Playlist[MusicTrackCount];

void Configure();

}  // namespace MusicMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto