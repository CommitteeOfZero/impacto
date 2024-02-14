#include "musicmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/musicmenu.h"
#include "../../../text.h"
#include "../../../game.h"
// #include "../../../window.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MusicMenu {

Sprite BackgroundSprite;

Sprite Thumbnails[MusicTrackCount];
glm::vec2 ThumbnailPosition;
Sprite ItemsWindow;
glm::vec2 ItemsWindowPosition;
RectF ItemsWindowRenderingBounds;
glm::vec2 MusicListMargin;
glm::vec2 MusicListInitialPosition;
Sprite PlaybackWindow;
glm::vec2 PlaybackWindowPosition;
Sprite PlaybackModeLabels[MusicPlaybackModeLabelCount];
glm::vec2 PlaybackModeLabelPosition;
glm::vec2 CurrentlyPlayingLabelPosition;
Sprite ItemNames[MusicTrackCount];
glm::vec2 ItemNameHighlightOffset;
Sprite LockedItem;
Sprite ScrollbarThumb;
Sprite ScrollbarTrack;
glm::vec2 ScrollbarPosition;
float ScrollbarStart;
int Playlist[MusicTrackCount];
Sprite TimerChars[TimerCharCount];
glm::vec2 TimerInitialPosition;
glm::vec2 TimerMargin;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  GetMemberSpriteArray(Thumbnails, MusicTrackCount, "Thumbnails");
  ThumbnailPosition = EnsureGetMemberVec2("ThumbnailPosition");
  ItemsWindow = EnsureGetMemberSprite("ItemsWindow");
  ItemsWindowPosition = EnsureGetMemberVec2("ItemsWindowPosition");
  ItemsWindowRenderingBounds =
      EnsureGetMemberRectF("ItemsWindowRenderingBounds");
  MusicListMargin = EnsureGetMemberVec2("MusicListMargin");
  MusicListInitialPosition = EnsureGetMemberVec2("MusicListInitialPosition");
  PlaybackWindow = EnsureGetMemberSprite("PlaybackWindow");
  PlaybackWindowPosition = EnsureGetMemberVec2("PlaybackWindowPosition");
  GetMemberSpriteArray(PlaybackModeLabels, MusicPlaybackModeLabelCount,
                       "PlaybackModeLabels");
  PlaybackModeLabelPosition = EnsureGetMemberVec2("PlaybackModeLabelPosition");
  CurrentlyPlayingLabelPosition =
      EnsureGetMemberVec2("CurrentlyPlayingLabelPosition");
  GetMemberSpriteArray(ItemNames, MusicTrackCount, "ItemNames");
  ItemNameHighlightOffset = EnsureGetMemberVec2("ItemNameHighlightOffset");
  LockedItem = EnsureGetMemberSprite("LockedItem");
  ScrollbarThumb = EnsureGetMemberSprite("ScrollbarThumb");
  ScrollbarTrack = EnsureGetMemberSprite("ScrollbarTrack");
  ScrollbarPosition = EnsureGetMemberVec2("ScrollbarPosition");
  ScrollbarStart = EnsureGetMemberFloat("ScrollbarStart");

  {
    EnsurePushMemberOfType("Playlist", LUA_TTABLE);

    auto size = lua_rawlen(LuaState, -1);
    assert(size == MusicTrackCount);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKeyInt() - 1;
      Playlist[i] = EnsureGetArrayElementInt();
      Pop();
    }

    Pop();
  }

  GetMemberSpriteArray(TimerChars, TimerCharCount, "TimerChars");
  TimerInitialPosition = EnsureGetMemberVec2("TimerInitialPosition");
  TimerMargin = EnsureGetMemberVec2("TimerMargin");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::MusicMenu());
}

}  // namespace MusicMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
