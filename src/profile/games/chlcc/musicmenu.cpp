#include "musicmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/musicmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MusicMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMember<float>("TransitionDuration");
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");
  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  TrackTree = EnsureGetMember<Sprite>("TrackTreeSprite");
  TrackTreePos = EnsureGetMember<glm::vec2>("TrackTreePos");
  TrackButtonPosTemplate = EnsureGetMember<glm::vec2>("TrackButtonPosTemplate");
  TrackNameOffset = EnsureGetMember<glm::vec2>("TrackNameOffset");
  ArtistOffset = EnsureGetMember<glm::vec2>("ArtistOffset");
  TrackOffset = EnsureGetMember<glm::vec2>("TrackOffset");
  TrackHighlight = EnsureGetMember<Sprite>("TrackHighlight");
  TrackNumRelativePos = EnsureGetMember<glm::vec2>("TrackNumRelativePos");
  PlaymodeRepeatPos = EnsureGetMember<glm::vec2>("PlaymodeRepeatPos");
  PlaymodeAllPos = EnsureGetMember<glm::vec2>("PlaymodeAllPos");
  PlaymodeRepeat = EnsureGetMember<Sprite>("PlaymodeRepeat");
  PlaymodeAll = EnsureGetMember<Sprite>("PlaymodeAll");
  PlaymodeRepeatHighlight = EnsureGetMember<Sprite>("PlaymodeRepeatHighlight");
  PlaymodeAllHighlight = EnsureGetMember<Sprite>("PlaymodeAllHighlight");
  NowPlaying = EnsureGetMember<Sprite>("NowPlaying");
  NowPlayingPos = EnsureGetMember<glm::vec2>("NowPlayingPos");
  NowPlayingAnimationDuration =
      EnsureGetMember<float>("NowPlayingAnimationDuration");
  PlayingTrackOffset = EnsureGetMember<glm::vec2>("PlayingTrackOffset");
  PlayingTrackArtistOffset =
      EnsureGetMember<glm::vec2>("PlayingTrackArtistOffset");
  SoundLibraryTitle = EnsureGetMember<Sprite>("SoundLibraryTitle");
  SoundLibraryTitleLeftPos =
      EnsureGetMember<glm::vec2>("SoundLibraryTitleLeftPos");
  SoundLibraryTitleRightPos =
      EnsureGetMember<glm::vec2>("SoundLibraryTitleRightPos");
  SoundLibraryTitleAngle = EnsureGetMember<float>("SoundLibraryTitleAngle");
  HighlightStar = EnsureGetMember<Sprite>("HighlightStar");
  HighlightStarRelativePos =
      EnsureGetMember<glm::vec2>("HighlightStarRelativePos");
  GetMemberArray<int>(Playlist, MusicTrackCount, "Playlist");
  GetMemberArray<Sprite>(SelectSound, 11, "SelectSound");
  GetMemberArray<glm::vec2>(SelectSoundPos, 11, "SelectSoundPos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::Menus[drawType].push_back(new UI::CHLCC::MusicMenu());
}

}  // namespace MusicMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto