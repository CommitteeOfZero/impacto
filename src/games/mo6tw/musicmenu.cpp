#include "musicmenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/musicmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../data/savesystem.h"
#include "../../audio/audiochannel.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::MusicMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

using namespace Impacto::Vm::Interface;

void MusicMenu::MusicButtonOnClick(Button* target) {
  if (target->IsLocked) return;

  SwitchToTrack(target->Id);
}

MusicMenu::MusicMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  MainItems = new Group(this);
  MainItems->WrapFocus = false;
  BackgroundItems = new Group(this);
  BackgroundItems->FocusLock = false;
  Timer = new Group(this);
  Timer->FocusLock = false;

  BackgroundItems->Add(new Label(ItemsWindow, ItemsWindowPosition));
  BackgroundItems->Add(new Label(PlaybackWindow, PlaybackWindowPosition));
  NullSprite = Sprite();
  NullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  Thumbnail = new Label(NullSprite, ThumbnailPosition);
  BackgroundItems->Add(Thumbnail);
  CurrentlyPlaying = new Label(NullSprite, CurrentlyPlayingLabelPosition);
  BackgroundItems->Add(CurrentlyPlaying);
  PlaybackModeLabel =
      new Label(PlaybackModeLabels[PlaybackMode], PlaybackModeLabelPosition);
  BackgroundItems->Add(PlaybackModeLabel);

  auto onClick = [this](auto* btn) { return MusicButtonOnClick(btn); };

  MainItems->RenderingBounds = ItemsWindowRenderingBounds;
  auto pos = MusicListInitialPosition;
  for (int i = 0; i < MusicTrackCount; i++) {
    auto button = new Button(i, ItemNames[i], ItemNames[i],
                             Profile::BacklogMenu::EntryHighlight, pos);
    button->HighlightOffset = ItemNameHighlightOffset;
    button->LockedSprite = LockedItem;
    button->OnClickHandler = onClick;
    MainItems->Add(button, FDIR_DOWN);

    pos += MusicListMargin;
  }

  MusicListY = ScrollbarStart;
  float totalHeight = MusicListMargin.y * (MusicTrackCount + 1);
  MainItems->Bounds =
      RectF(MusicListInitialPosition.x, MusicListInitialPosition.y,
            ItemNames[0].ScaledWidth(), totalHeight);
  auto scrollbar = new Scrollbar(
      0, ScrollbarPosition, ScrollbarStart,
      ScrollbarStart - totalHeight + ItemsWindowRenderingBounds.Height,
      &MusicListY, SBDIR_VERTICAL, ScrollbarTrack, ScrollbarThumb);
  BackgroundItems->Add(scrollbar);

  pos = TimerInitialPosition;
  for (int i = 0; i < 8; i++) {
    Timer->Add(new Label(NullSprite, pos));
    pos += TimerMargin;
  }
}

void MusicMenu::Show() {
  if (State != Shown) {
    State = Showing;
    UpdateMusicEntries();
    FadeAnimation.StartIn();
    MainItems->Show();
    BackgroundItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void MusicMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems->Hide();
    BackgroundItems->Hide();
    Timer->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void MusicMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();
    BackgroundItems->UpdateInput();
    if ((PADinputButtonWentDown & PAD1DOWN ||
         PADinputButtonWentDown & PAD1UP) &&
        MainItems->HasFocus) {
      auto focusedEl = CurrentlyFocusedElement;
      if (focusedEl->Bounds.Y < MainItems->RenderingBounds.Y) {
        MusicListY += MusicListMargin.y;
      } else if (focusedEl->Bounds.Y + focusedEl->Bounds.Height >
                 MainItems->RenderingBounds.Y +
                     MainItems->RenderingBounds.Height) {
        MusicListY -= MusicListMargin.y;
      }
    }

    if (PADinputButtonWentDown & PAD1Y) {
      auto mode = (int)PlaybackMode + 1;
      if (mode > 3) mode = 0;
      PlaybackMode = (MusicPlaybackMode)mode;
      if (PlaybackMode == MPM_RepeatOne) {
        Audio::Channels[Audio::AC_BGM0]->Looping = true;
      } else {
        Audio::Channels[Audio::AC_BGM0]->Looping = false;
      }
      PlaybackModeLabel->SetSprite(PlaybackModeLabels[PlaybackMode]);
    }

    if (PADinputButtonWentDown & PAD1X) {
      SwitchToTrack(-1);
    }
  }
}

void MusicMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_MUSICMODE_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_MUSICMODE_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_MUSICMODE_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    BackgroundItems->Update(dt);
    MainItems->Update(dt);
    MainItems->MoveTo(glm::vec2(MainItems->Bounds.X, MusicListY));

    UpdateMusicTimer();
    if (CurrentlyPlayingTrackId != -1 &&
        Audio::Channels[Audio::AC_BGM0]->State == Audio::ACS_Stopped) {
      int trackId;
      if (PlaybackMode == MPM_One) {
        trackId = -1;
      } else {
        trackId = GetNextTrackId(CurrentlyPlayingTrackId + 1);
        if (trackId == MusicTrackCount) {
          if (PlaybackMode == MPM_RepeatPlaylist) {
            trackId = GetNextTrackId(0);
          } else if (PlaybackMode == MPM_Playlist) {
            trackId = -1;
          }
        }
      }
      SwitchToTrack(trackId);
    }
  }
}

void MusicMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    BackgroundItems->Tint = col;
    BackgroundItems->Render();
    MainItems->Tint = col;
    MainItems->Render();
    Timer->Tint.a *= col.a;
    Timer->Render();
  }
}

void MusicMenu::UpdateMusicEntries() {
  for (auto el : MainItems->Children) {
    auto button = static_cast<Button*>(el);
    button->IsLocked = !SaveSystem::GetBgmFlag(Playlist[button->Id]);
  }
}

void MusicMenu::UpdateMusicTimer() {
  if (Audio::Channels[Audio::AC_BGM0]->State == Audio::ACS_Stopped) {
    Timer->Hide();
  } else {
    int position = (int)Audio::Channels[Audio::AC_BGM0]->PositionInSeconds();
    auto seconds = position % 3600 % 60;
    auto minutes = position % 3600 / 60;
    auto hours = position / 3600;
    std::string time =
        fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, seconds);
    for (int i = 0; i < 8; i++) {
      auto label = static_cast<Label*>(Timer->Children.at(i));
      label->SetSprite(time[i] == ':' ? TimerChars[TimerCharCount - 1]
                                      : TimerChars[time[i] - '0']);
    }
  }
}

void MusicMenu::SwitchToTrack(int id) {
  CurrentlyPlayingTrackId = id;
  if (id == -1) {
    Audio::Channels[Audio::AC_BGM0]->Stop(0.5f);
    Thumbnail->SetSprite(NullSprite);
    CurrentlyPlaying->SetSprite(NullSprite);
    Timer->Hide();
    return;
  }

  Audio::Channels[Audio::AC_BGM0]->Play("bgm", Playlist[id],
                                        PlaybackMode == MPM_RepeatOne, 0.5f);
  Thumbnail->SetSprite(Thumbnails[id]);
  CurrentlyPlaying->SetSprite(ItemNames[id]);
  Timer->Show();
}

inline int MusicMenu::GetNextTrackId(int id) {
  while (!SaveSystem::GetBgmFlag(Playlist[id])) {
    id += 1;
    if (id == MusicTrackCount) {
      if (PlaybackMode == MPM_RepeatPlaylist) {
        id = 0;
      } else if (PlaybackMode == MPM_Playlist) {
        id = -1;
        break;
      }
    }
  }
  return id;
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto