#include "musicmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/musicmenu.h"
#include "../../../text.h"
#include "../../../game.h"
//#include "../../../window.h"
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
    EnsurePushMemberOfType("Playlist", kArrayType);

    auto const& _items = TopVal();
    auto size = _items.Size();
    assert(size == MusicTrackCount);
    for (uint32_t i = 0; i < MusicTrackCount; i++) {
      Playlist[i] = EnsureGetArrayElementInt(i);
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
