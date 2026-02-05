#include "musicmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/musicmenu.h"
#include "../../../text/text.h"
#include "../../../game.h"
// #include "../../../window.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MusicMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  GetMemberArray<Sprite>(Thumbnails, MusicTrackCount, "Thumbnails");
  ThumbnailPosition = EnsureGetMember<glm::vec2>("ThumbnailPosition");
  ItemsWindow = EnsureGetMember<Sprite>("ItemsWindow");
  ItemsWindowPosition = EnsureGetMember<glm::vec2>("ItemsWindowPosition");
  ItemsWindowRenderingBounds =
      EnsureGetMember<RectF>("ItemsWindowRenderingBounds");
  MusicListMargin = EnsureGetMember<glm::vec2>("MusicListMargin");
  MusicListInitialPosition =
      EnsureGetMember<glm::vec2>("MusicListInitialPosition");
  PlaybackWindow = EnsureGetMember<Sprite>("PlaybackWindow");
  PlaybackWindowPosition = EnsureGetMember<glm::vec2>("PlaybackWindowPosition");
  GetMemberArray<Sprite>(PlaybackModeLabels, MusicPlaybackModeLabelCount,
                         "PlaybackModeLabels");
  PlaybackModeLabelPosition =
      EnsureGetMember<glm::vec2>("PlaybackModeLabelPosition");
  CurrentlyPlayingLabelPosition =
      EnsureGetMember<glm::vec2>("CurrentlyPlayingLabelPosition");
  GetMemberArray<Sprite>(ItemNames, MusicTrackCount, "ItemNames");
  ItemNameHighlightOffset =
      EnsureGetMember<glm::vec2>("ItemNameHighlightOffset");
  LockedItem = EnsureGetMember<Sprite>("LockedItem");
  ScrollbarThumb = EnsureGetMember<Sprite>("ScrollbarThumb");
  ScrollbarTrack = EnsureGetMember<Sprite>("ScrollbarTrack");
  ScrollbarPosition = EnsureGetMember<glm::vec2>("ScrollbarPosition");
  ScrollbarStart = EnsureGetMember<float>("ScrollbarStart");

  {
    EnsurePushMemberOfType("Playlist", LUA_TTABLE);

    [[maybe_unused]] auto size = lua_rawlen(LuaState, -1);
    assert(size == MusicTrackCount);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKey<int32_t>() - 1;
      Playlist[i] = EnsureGetArrayElement<int>();
      Pop();
    }

    Pop();
  }

  GetMemberArray<Sprite>(TimerChars, TimerCharCount, "TimerChars");
  TimerInitialPosition = EnsureGetMember<glm::vec2>("TimerInitialPosition");
  TimerMargin = EnsureGetMember<glm::vec2>("TimerMargin");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::MusicMenu());
}

}  // namespace MusicMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
