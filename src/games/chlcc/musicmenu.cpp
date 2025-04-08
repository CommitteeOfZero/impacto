#include "musicmenu.h"

#include "../../profile/games/chlcc/musicmenu.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"
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
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.SetDuration(MenuTransitionDuration);

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  NowPlayingAnimation.Direction = AnimationDirection::In;
  NowPlayingAnimation.LoopMode = AnimationLoopMode::Stop;
  NowPlayingAnimation.SetDuration(NowPlayingAnimationDuration);

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  PlaymodeRepeatSprite = PlaymodeRepeat;
  PlaymodeAllSprite = PlaymodeAll;

  MainItems = new Group(this);

  for (int idx = 0; idx < MusicTrackCount; idx++) {
    auto button = new Widgets::CHLCC::TrackSelectButton(
        idx, TrackHighlight, TrackButtonPosTemplate + (float)idx * TrackOffset,
        TrackNumRelativePos, TrackNameOffset, ArtistOffset);
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
    CurrentlyPlayingTrackName.Show();
    CurrentlyPlayingTrackArtist.Show();
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
    CurrentlyPlayingTrackId = -1;
    NowPlayingAnimation.StartOut();
    Audio::Channels[Audio::AC_BGM0]->Stop(0.0f);
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

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);
      Renderer->DrawSprite(SoundLibraryTitle, RightTitlePos, glm::vec4(1.0f),
                           glm::vec2(1.0f), SoundLibraryTitleAngle);
    }

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

        glm::vec2 currentScroll(0.0f,
                                -(float)CurrentLowerBound * TrackOffset.y);
        MainItems->MoveTo(currentScroll + offset);
        for (auto button : MainItems->Children)
          static_cast<Widgets::CHLCC::TrackSelectButton*>(button)->MoveTracks(
              currentScroll + offset);
      }
      MainItems->Render();

      if (MenuTransition.Progress > 0.34f) {
        Renderer->DrawSprite(SoundLibraryTitle, LeftTitlePos);
      }

      for (int idx = 0; idx < 11; idx++) {
        Renderer->DrawSprite(SelectSound[idx], SelectSoundPos[idx] + offset);
      }

      Renderer->DrawSprite(TrackTree, TrackTreePos + offset);
      Renderer->DrawSprite(PlaymodeRepeatSprite, PlaymodeRepeatPos + offset);
      Renderer->DrawSprite(PlaymodeAllSprite, PlaymodeAllPos + offset);
      glm::vec2 temp =
          glm::vec2(15 * 16 * (1 - NowPlayingAnimation.Progress), offset.y) +
          NowPlayingPos;
      glm::vec4 tint(glm::vec3(1.0f), NowPlayingAnimation.Progress);
      CurrentlyPlayingTrackName.MoveTo(temp + PlayingTrackOffset);
      CurrentlyPlayingTrackName.Tint = tint;
      CurrentlyPlayingTrackName.Render();
      CurrentlyPlayingTrackArtist.MoveTo(temp + PlayingTrackArtistOffset);
      CurrentlyPlayingTrackArtist.Tint = tint;
      CurrentlyPlayingTrackArtist.Render();
      Renderer->DrawSprite(NowPlaying, temp, tint);
      if (CurrentlyPlayingTrackId != -1 &&
          CurrentlyPlayingTrackId >= CurrentLowerBound &&
          CurrentlyPlayingTrackId <= CurrentUpperBound) {
        Renderer->DrawSprite(
            HighlightStar,
            glm::vec2(MainItems->Children[CurrentlyPlayingTrackId]->Bounds.X,
                      MainItems->Children[CurrentlyPlayingTrackId]->Bounds.Y) +
                HighlightStarRelativePos);
      }
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
    CurrentlyPlayingTrackName.Hide();
    CurrentlyPlayingTrackArtist.Hide();
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
    if (MenuTransition.Direction == +AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == +AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    NowPlayingAnimation.Update(dt);
    UpdateTitles();
    if (InputEnabled) MainItems->Update(dt);
    if (CurrentlyPlayingTrackId == -1) return;
    if (PlaybackMode != MPM_RepeatOne && CurrentlyPlayingTrackId < 40 &&
        abs(Audio::Channels[Audio::AC_BGM0]->PositionInSeconds() -
            PreviousPosition) > 1.0f) {
      CurrentlyPlayingTrackId = -1;
      NowPlayingAnimation.StartOut();
      Audio::Channels[Audio::AC_BGM0]->Stop(2.0f);
    }
    if (Audio::Channels[Audio::AC_BGM0]->State == Audio::ACS_Stopped) {
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
    PreviousPosition = Audio::Channels[Audio::AC_BGM0]->PositionInSeconds();
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
        Audio::Channels[Audio::AC_BGM0]->Looping = true;
      } else {
        Audio::Channels[Audio::AC_BGM0]->Looping = false;
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
  auto onClick = [this](auto* btn) { return MusicButtonOnClick(btn); };

  for (size_t idx = 0; idx < MainItems->Children.size(); idx++) {
    auto button = static_cast<Widgets::CHLCC::TrackSelectButton*>(
        MainItems->Children[idx]);

    if (!SaveSystem::GetBgmFlag((int)idx)) {
      button->SetTrackText(Vm::ScriptGetTextTableStrAddress(0, 15));
      continue;
    }

    button->OnClickHandler = onClick;
    button->SetTrackText(Vm::ScriptGetTextTableStrAddress(4, (int)idx * 3));
    button->SetArtistText(
        Vm::ScriptGetTextTableStrAddress(4, (int)idx * 3 + 1));
  }
}

void MusicMenu::SwitchToTrack(int id) {
  CurrentlyPlayingTrackId = id;
  if (id == -1) {
    NowPlayingAnimation.StartOut();
    Audio::Channels[Audio::AC_BGM0]->Stop(0.5f);
    return;
  }
  if (!NowPlayingAnimation.IsIn()) NowPlayingAnimation.StartIn();
  CurrentlyPlayingTrackName = Label(
      Vm::ScriptGetTextTableStrAddress(4, CurrentlyPlayingTrackId * 3),
      NowPlayingPos + PlayingTrackOffset, 32, RendererOutlineMode::None, 0);
  CurrentlyPlayingTrackArtist = Label(
      Vm::ScriptGetTextTableStrAddress(4, CurrentlyPlayingTrackId * 3 + 2),
      NowPlayingPos + PlayingTrackArtistOffset, 20, RendererOutlineMode::None,
      0);
  PreviousPosition = 0.0f;
  Audio::Channels[Audio::AC_BGM0]->Play(
      "bgm", Playlist[id], id >= 40 ? (PlaybackMode == MPM_RepeatOne) : true,
      0.5f);
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

void MusicMenu::UpdateTitles() {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = SoundLibraryTitleRightPos;
  LeftTitlePos = glm::vec2(
      SoundLibraryTitleLeftPos.x,
      TitleFade.IsIn()
          ? SoundLibraryTitleLeftPos.y
          : glm::mix(
                1.0f, 721.0f,
                1.01011f * std::sin(1.62223f * TitleFade.Progress + 3.152f) +
                    1.01012f));

  if (MenuTransition.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto