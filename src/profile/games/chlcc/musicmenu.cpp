#include "musicmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/musicmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MusicMenu {

float MenuTransitionDuration;
uint32_t BackgroundColor;
Sprite CircleSprite;
glm::vec2 CircleStartPosition;
float CircleOffset;
glm::vec2 ErinPosition;
Sprite ErinSprite;
Sprite BackgroundFilter;
glm::vec2 InitialRedBarPosition;
glm::vec2 RightRedBarPosition;
Sprite InitialRedBarSprite;
glm::vec2 RedBarPosition;
float RedBarDivision;
float RedBarBaseX;
Sprite RedBarSprite;
glm::vec2 RedBarLabelPosition;
Sprite RedBarLabel;
float TitleFadeInDuration;
float TitleFadeOutDuration;
Sprite TrackTree;
glm::vec2 TrackTreePos;
glm::vec2 TrackButtonPosTemplate;
glm::vec2 TrackNameOffset;
glm::vec2 ArtistOffset;
glm::vec2 TrackOffset;
Sprite TrackHighlight;
glm::vec2 TrackNumRelativePos;
glm::vec2 PlaymodeRepeatPos;
glm::vec2 PlaymodeAllPos;
Sprite PlaymodeRepeat;
Sprite PlaymodeAll;
Sprite PlaymodeRepeatHighlight;
Sprite PlaymodeAllHighlight;
Sprite NowPlaying;
glm::vec2 NowPlayingPos;
float NowPlayingAnimationDuration;
glm::vec2 PlayingTrackOffset;
glm::vec2 PlayingTrackArtistOffset;
Sprite SoundLibraryTitle;
glm::vec2 SoundLibraryTitleLeftPos;
glm::vec2 SoundLibraryTitleRightPos;
float SoundLibraryTitleAngle;
Sprite HighlightStar;
glm::vec2 HighlightStarRelativePos;
int Playlist[MusicTrackCount];
Sprite SelectSound[11];
glm::vec2 SelectSoundPos[11];

static void GetMemberVec2Array(glm::vec2* arr, uint32_t count,
                               char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d vec2 for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementVec2(i);
  }

  Pop();
}

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

static void GetMemberIntArray(int* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d ints for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementInt(i);
  }

  Pop();
}

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