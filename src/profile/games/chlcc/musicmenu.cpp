#include "musicmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/musicmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MusicMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMemberFloat("TransitionDuration");
  BackgroundColor = EnsureGetMemberUint("BackgroundColor");
  CircleSprite = EnsureGetMemberSprite("CircleSprite");
  CircleStartPosition = EnsureGetMemberVec2("CircleStartPosition");
  CircleOffset = EnsureGetMemberFloat("CircleOffset");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMemberVec2("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMemberVec2("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMemberSprite("RedBarSprite");
  RedBarDivision = EnsureGetMemberFloat("RedBarDivision");
  RedBarBaseX = EnsureGetMemberFloat("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMemberVec2("RedBarLabelPosition");
  RedBarLabel = EnsureGetMemberSprite("RedBarLabel");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  TrackTree = EnsureGetMemberSprite("TrackTreeSprite");
  TrackTreePos = EnsureGetMemberVec2("TrackTreePos");
  TrackButtonPosTemplate = EnsureGetMemberVec2("TrackButtonPosTemplate");
  TrackNameOffset = EnsureGetMemberVec2("TrackNameOffset");
  ArtistOffset = EnsureGetMemberVec2("ArtistOffset");
  TrackOffset = EnsureGetMemberVec2("TrackOffset");
  TrackHighlight = EnsureGetMemberSprite("TrackHighlight");
  TrackNumRelativePos = EnsureGetMemberVec2("TrackNumRelativePos");
  PlaymodeRepeatPos = EnsureGetMemberVec2("PlaymodeRepeatPos");
  PlaymodeAllPos = EnsureGetMemberVec2("PlaymodeAllPos");
  PlaymodeRepeat = EnsureGetMemberSprite("PlaymodeRepeat");
  PlaymodeAll = EnsureGetMemberSprite("PlaymodeAll");
  PlaymodeRepeatHighlight = EnsureGetMemberSprite("PlaymodeRepeatHighlight");
  PlaymodeAllHighlight = EnsureGetMemberSprite("PlaymodeAllHighlight");
  NowPlaying = EnsureGetMemberSprite("NowPlaying");
  NowPlayingPos = EnsureGetMemberVec2("NowPlayingPos");
  NowPlayingAnimationDuration =
      EnsureGetMemberFloat("NowPlayingAnimationDuration");
  PlayingTrackOffset = EnsureGetMemberVec2("PlayingTrackOffset");
  PlayingTrackArtistOffset = EnsureGetMemberVec2("PlayingTrackArtistOffset");
  SoundLibraryTitle = EnsureGetMemberSprite("SoundLibraryTitle");
  SoundLibraryTitleLeftPos = EnsureGetMemberVec2("SoundLibraryTitleLeftPos");
  SoundLibraryTitleRightPos = EnsureGetMemberVec2("SoundLibraryTitleRightPos");
  SoundLibraryTitleAngle = EnsureGetMemberFloat("SoundLibraryTitleAngle");
  HighlightStar = EnsureGetMemberSprite("HighlightStar");
  HighlightStarRelativePos = EnsureGetMemberVec2("HighlightStarRelativePos");
  GetMemberIntArray(Playlist, MusicTrackCount, "Playlist");
  GetMemberSpriteArray(SelectSound, 11, "SelectSound");
  GetMemberVec2Array(SelectSoundPos, 11, "SelectSoundPos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::CHLCC::MusicMenu());
}

}  // namespace MusicMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto