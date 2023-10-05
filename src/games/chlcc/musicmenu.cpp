#include "musicmenu.h"

#include "../../profile/games/chlcc/musicmenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"
#include "../../profile/dialogue.h"
#include "../../background2d.h"
#include "../../vm/interface/input.h"

#include "../../ui/widgets/chlcc/trackselectbutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::MusicMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

using namespace Impacto::Vm::Interface;

void MusicMenu::MusicButtonOnClick(Button* target) {
  if (target->IsLocked) return;

  SwitchToTrack(target->Id);
}

MusicMenu::MusicMenu() {
  MenuTransition.Direction = 1.0f;
  MenuTransition.LoopMode = ALM_Stop;
  MenuTransition.DurationIn = MenuTransitionDuration;
  MenuTransition.DurationOut = MenuTransitionDuration;

  TitleFade.Direction = 1.0f;
  TitleFade.LoopMode = ALM_Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  PlaymodeRepeatSprite = PlaymodeRepeat;
  PlaymodeAllSprite = PlaymodeAll;

  MainItems = new Group(this);
  Sprite empty = Sprite();

  auto onClick =
      std::bind(&MusicMenu::MusicButtonOnClick, this, std::placeholders::_1);

  for (int idx = 0; idx < MusicTrackCount; idx++) {
    auto button = new Widgets::CHLCC::TrackSelectButton(
        idx, TrackHighlight, TrackButtonPosTemplate + (float)idx * TrackOffset,
        TrackNumRelativePos, TrackNameOffset, ArtistOffset);
    button->OnClickHandler = onClick;
    MainItems->Add(button, FDIR_DOWN);
  }

  MainItems->Children.front()->SetFocus(MainItems->Children.back(), FDIR_UP);
  MainItems->Children.back()->SetFocus(MainItems->Children.front(), FDIR_DOWN);
}

void MusicMenu::Show() {
  if (State != Shown) {
    if (State != Showing) MenuTransition.StartIn();
    State = Showing;
    UpdateEntries();
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    MainItems->Children.front()->HasFocus = true;
    CurrentlyFocusedElement = MainItems->Children.front();
  }
}

void MusicMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
    }
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
    InputEnabled = false;
  }
}

void MusicMenu::Render() {
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawRect(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();
    DrawRedBar();

    Renderer->CaptureScreencap(ShaderScreencapture.BgSprite);
    Renderer->DrawCHLCCMenuBackground(
        ShaderScreencapture.BgSprite, BackgroundFilter,
        RectF(0.0f, 0.0f, 1280.0f, 720.0f), MenuTransition.Progress);

    glm::vec2 offset(0.0f, 0.0f);
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.73f) {
        // Approximated function from the original, another mess
        offset = glm::vec2(
            0.0f,
            glm::mix(-720.0f, 0.0f,
                     1.00397f * std::sin(3.97161f -
                                         3.26438f * MenuTransition.Progress) -
                         0.00295643f));

        MainItems->RenderingBounds =
            RectF(0.0f, TrackButtonPosTemplate.y + offset.y, 1280.0f,
                  16 * TrackOffset.y + 1);
        MainItems->MoveTo(offset);
        for (auto button : MainItems->Children)
          static_cast<Widgets::CHLCC::TrackSelectButton*>(button)->MoveTracks(
              offset);
      }
      MainItems->Render();
      Renderer->DrawSprite(TrackTree, TrackTreePos + offset);
      Renderer->DrawSprite(PlaymodeRepeatSprite, PlaymodeRepeatPos + offset);
      Renderer->DrawSprite(PlaymodeAllSprite, PlaymodeAllPos + offset);
    }
  }
}

void MusicMenu::Update(float dt) {
  UpdateInput();
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_SOUNDMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && State == Hiding) {
    MainItems->Hide();
    State = Hidden;
  } else if (MenuTransition.IsIn() && State == Showing) {
    State = Shown;
    MainItems->RenderingBounds =
        RectF(0.0f, TrackButtonPosTemplate.y, 1280.0f, 16 * TrackOffset.y + 1);
    MainItems->MoveTo({0.0f, 0.0f});
    for (auto el : MainItems->Children)
      static_cast<Widgets::CHLCC::TrackSelectButton*>(el)->MoveTracks(
          {0.0f, 0.0f});
    InputEnabled = true;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == -1.0f && MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == 1.0f || TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    if (InputEnabled) MainItems->Update(dt);
    if (CurrentlyPlayingTrackId == -1) return;
    if (PlaybackMode != MPM_RepeatOne && CurrentlyPlayingTrackId < 40 &&
        abs(Audio::Channels[Audio::AC_BGM0].PositionInSeconds() -
            PreviousPosition) > 1.0f) {
      Audio::Channels[Audio::AC_BGM0].Stop(2.0f);
    }
    if (Audio::Channels[Audio::AC_BGM0].State == Audio::ACS_Stopped) {
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
    PreviousPosition = Audio::Channels[Audio::AC_BGM0].PositionInSeconds();
  }
}

void MusicMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();

    if (PADinputButtonWentDown & PAD1Y) {
      auto mode = (int)PlaybackMode + 1;
      if (mode > 3) mode = 0;
      PlaybackMode = (MusicPlaybackMode)mode;
      PlaymodeAllSprite = mode & 1 ? PlaymodeAllHighlight : PlaymodeAll;
      PlaymodeRepeatSprite =
          mode & 2 ? PlaymodeRepeatHighlight : PlaymodeRepeat;
      if (PlaybackMode == MPM_RepeatOne) {
        Audio::Channels[Audio::AC_BGM0].Looping = true;
      } else {
        Audio::Channels[Audio::AC_BGM0].Looping = false;
      }
    }

    if (PADinputButtonWentDown & PAD1X) {
      SwitchToTrack(-1);
    }

    auto button = static_cast<Widgets::CHLCC::TrackSelectButton*>(
        CurrentlyFocusedElement);
    if (button == nullptr) return;

    // Opposite of scroll direction
    if (button->Id - CurrentLowerBound >= 16) {
      CurrentLowerBound = button->Id - 15;
      CurrentUpperBound = button->Id;
    } else if (CurrentUpperBound - button->Id >= 16) {
      CurrentLowerBound = button->Id;
      CurrentUpperBound = button->Id + 15;
    } else
      return;

    glm::vec2 offset(0.0f, -(float)CurrentLowerBound * TrackOffset.y);
    MainItems->MoveTo(offset);
    for (auto el : MainItems->Children) {
      auto b = static_cast<Widgets::CHLCC::TrackSelectButton*>(el);
      b->MoveTracks(offset);
    }
  }
}

inline void MusicMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        scale *= CircleSprite.Bounds.Height / 106.0f;
        Renderer->DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

inline void MusicMenu::DrawErin() {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = 801.0f;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger
      // mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void MusicMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}

void MusicMenu::UpdateEntries() {
  for (int idx = 0; idx < MainItems->Children.size(); idx++) {
    auto button = static_cast<Widgets::CHLCC::TrackSelectButton*>(
        MainItems->Children[idx]);
    button->SetTrackText(Vm::ScriptGetTextTableStrAddress(4, idx * 3));
    button->SetArtistText(Vm::ScriptGetTextTableStrAddress(4, idx * 3 + 1));
  }
}

void MusicMenu::SwitchToTrack(int id) {
  CurrentlyPlayingTrackId = id;
  if (id == -1) {
    Audio::Channels[Audio::AC_BGM0].Stop(0.5f);
    return;
  }
  Io::InputStream* stream;
  Io::VfsOpen("bgm", Playlist[id], &stream);
  PreviousPosition = 0.0f;
  Audio::Channels[Audio::AC_BGM0].Play(
      Audio::AudioStream::Create(stream),
      id >= 40 ? (PlaybackMode == MPM_RepeatOne) : true, 0.5f);
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

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto