#include "musicmenu.h"

#include "../../profile/games/chlcc/musicmenu.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"
#include "../../background2d.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/chlcc/trackselectbutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::MusicMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

using namespace Impacto::Vm::Interface;

constexpr bool isRepeat(MusicPlaybackMode playbackMode) {
  return (playbackMode & MusicPlaybackMode::RepeatOne) ==
         MusicPlaybackMode::RepeatOne;
}

constexpr bool isPlaylist(MusicPlaybackMode playbackMode) {
  return (playbackMode & MusicPlaybackMode::Playlist) ==
         MusicPlaybackMode::Playlist;
}

void MusicMenu::MusicButtonOnClick(Button* target) {
  if (target->IsLocked) return;

  SwitchToTrack(target->Id);
}

void MusicMenu::ToggleRepeatMode() {
  PlaybackMode ^= MusicPlaybackMode::RepeatOne;
  PlayModeRepeatSprite =
      isRepeat(PlaybackMode) ? PlaymodeRepeatHighlight : PlaymodeRepeat;
  UpdateLooping();
}

void MusicMenu::ToggleAllMode() {
  PlaybackMode ^= MusicPlaybackMode::Playlist;
  PlayModeAllSprite =
      isPlaylist(PlaybackMode) ? PlaymodeAllHighlight : PlaymodeAll;
  UpdateLooping();
}

void MusicMenu::UpdateLooping() {
  if (PlaybackMode == MusicPlaybackMode::RepeatOne) {
    Audio::Channels[Audio::AC_BGM0]->SetLooping(true);
  } else {
    Audio::Channels[Audio::AC_BGM0]->SetLooping(false);
  }
}

static float GetEndScroll(Group* itemsGroup) {
  if (itemsGroup->Children.empty() || itemsGroup->Children.size() == 1)
    return 0.0f;
  const Widget* lastItem = itemsGroup->Children.back();
  const float lastItemEndPos = lastItem->Bounds.Height + lastItem->Bounds.Y;
  const float pagePos =
      lastItemEndPos - TrackListBounds.Height - TrackListBounds.Y;
  if (pagePos < 0.0f) return 0.0f;
  return round(pagePos / TrackOffset.y) * TrackOffset.y;
}

MusicMenu::MusicMenu() {
  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  NowPlayingAnimation.Direction = AnimationDirection::In;
  NowPlayingAnimation.LoopMode = AnimationLoopMode::Stop;
  NowPlayingAnimation.SetDuration(NowPlayingAnimationDuration);

  FromSystemMenuTransition.Direction = AnimationDirection::In;
  FromSystemMenuTransition.LoopMode = AnimationLoopMode::Stop;
  FromSystemMenuTransition.DurationIn = TitleFadeInDuration;
  FromSystemMenuTransition.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  auto toggleRepeatClick = [this](auto* btn) { return ToggleRepeatMode(); };
  PlayModeRepeatClickArea =
      ClickArea(0,
                {PlaymodeRepeatPos.x, PlaymodeRepeatPos.y,
                 PlaymodeRepeat.ScaledWidth(), PlaymodeRepeat.ScaledHeight()},
                toggleRepeatClick);

  auto toggleAllClick = [this](auto* btn) { return ToggleAllMode(); };
  PlayModeAllClickArea =
      ClickArea(1,
                {PlaymodeAllPos.x, PlaymodeAllPos.y, PlaymodeAll.ScaledWidth(),
                 PlaymodeAll.ScaledHeight()},
                toggleAllClick);

  MainItems = new Group(this);

  for (int idx = 0; idx < MusicTrackCount; idx++) {
    auto button = new Widgets::CHLCC::TrackSelectButton(
        idx, TrackHighlight, TrackButtonPosTemplate + (float)idx * TrackOffset,
        TrackNumRelativePos, TrackNameOffset, ArtistOffset);
    MainItems->Add(button, FDIR_DOWN);

    // Page scrolling
    if (idx >= SelectableItemsPerPage) {
      button->SetFocus(MainItems->Children[idx - SelectableItemsPerPage],
                       FDIR_LEFT);
      MainItems->Children[idx - SelectableItemsPerPage]->SetFocus(button,
                                                                  FDIR_RIGHT);
    } else {
      button->SetFocus(MainItems->Children.front(), FDIR_LEFT);
    }
  }

  MainScrollbar = Scrollbar(
      0, ScrollbarPosition, 0.0f, GetEndScroll(MainItems), &ScrollY,
      ScrollbarDirection::SBDIR_VERTICAL, ScrollThumbSprite, ScrollTrackBounds,
      ScrollThumbSprite.ScaledHeight(), TrackListBounds, 1.0f);
  MainScrollbar.Step = TrackOffset.y;

  // Everything in last page points to last element
  for (int idx = MusicTrackCount - 1 - SelectableItemsPerPage;
       idx < MusicTrackCount; idx++) {
    MainItems->Children[idx]->SetFocus(MainItems->Children.back(), FDIR_RIGHT);
  }

  MainItems->Children.front()->SetFocus(MainItems->Children.back(), FDIR_UP);
  MainItems->Children.front()->SetFocus(MainItems->Children.back(), FDIR_LEFT);
  MainItems->Children.back()->SetFocus(MainItems->Children.front(), FDIR_DOWN);
  MainItems->Children.back()->SetFocus(MainItems->Children.front(), FDIR_RIGHT);
}

void MusicMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      FromSystemMenuTransition.StartIn();
      MenuTransition.StartIn();
      SelectAnimation.StartIn(true);
    }
    State = Showing;
    UpdateEntries();
    MainItems->Show();
    CurrentlyPlayingTrackName.Show();
    CurrentlyPlayingTrackArtist.Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    ScrollY = 0.0f;
    IsFocused = true;
    UI::FocusedMenu = this;
    MainItems->Children.front()->HasFocus = true;
    CurrentlyFocusedElement = MainItems->Children.front();
    PlayModeRepeatSprite = PlaymodeRepeat;
    PlayModeAllSprite = PlaymodeAll;
    PlaybackMode = MusicPlaybackMode::One;
  }
}

void MusicMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
      FromSystemMenuTransition.StartOut();
      MenuTransition.StartOut();
    }
    CurrentlyPlayingTrackId.reset();
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
  if (State == Hidden) return;

  if (MenuTransition.IsIn()) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        RgbIntToFloat(BackgroundColor));
  } else if (GetFlag(SF_SYSTEMMENU)) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        RgbIntToFloat(BackgroundColor, FromSystemMenuTransition.Progress));
  } else {
    DrawCircles();
  }
  DrawErin();
  DrawRedBar();

  if (MenuTransition.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

    const CornersQuad titleDest =
        SoundLibraryTitle.ScaledBounds()
            .RotateAroundCenter(SoundLibraryTitleAngle)
            .Translate(RightTitlePos);
    Renderer->DrawSprite(SoundLibraryTitle, titleDest);
  }

  Renderer->CaptureScreencap(ShaderScreencapture.BgSprite);
  Renderer->DrawCHLCCMenuBackground(
      ShaderScreencapture.BgSprite, BackgroundFilter,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      MenuTransition.Progress);

  if (MenuTransition.Progress <= 0.22f) return;

  glm::vec2 offset = MenuTransition.GetPageOffset();
  if (MenuTransition.IsPlaying()) {
    glm::vec2 offsetScroll = glm::vec2(0.0f, -ScrollY) + offset;

    const RectF bounds =
        RectF(0.0f, TrackButtonPosTemplate.y + offset.y, Profile::DesignWidth,
              VisibleItemsPerPage * TrackOffset.y);
    MainItems->RenderingBounds = bounds;
    MainItems->HoverBounds = bounds;

    MainScrollbar.MoveTo(ScrollbarPosition + offset);

    MainItems->MoveTo(offsetScroll);
    for (auto button : MainItems->Children)
      static_cast<Widgets::CHLCC::TrackSelectButton*>(button)->MoveTracks(
          offsetScroll);
  }
  MainItems->Render();
  MainScrollbar.Render();

  if (MenuTransition.Progress > 0.34f) {
    Renderer->DrawSprite(SoundLibraryTitle, LeftTitlePos);
  }

  SelectAnimation.Draw(SelectSoundSprites, SelectSoundPos, offset);

  Renderer->DrawSprite(TrackTree, TrackTreePos + offset);
  Renderer->DrawSprite(PlayModeRepeatSprite, PlaymodeRepeatPos + offset);
  Renderer->DrawSprite(PlayModeAllSprite, PlaymodeAllPos + offset);
  glm::vec2 nowPlayingOffset =
      glm::vec2(15 * 16 * (1 - NowPlayingAnimation.Progress), offset.y) +
      NowPlayingPos;
  glm::vec4 tint(glm::vec3(1.0f), NowPlayingAnimation.Progress);
  CurrentlyPlayingTrackName.MoveTo(nowPlayingOffset + PlayingTrackOffset);
  CurrentlyPlayingTrackName.Tint = tint;
  CurrentlyPlayingTrackName.Render();
  CurrentlyPlayingTrackArtist.MoveTo(nowPlayingOffset +
                                     PlayingTrackArtistOffset);
  CurrentlyPlayingTrackArtist.Tint = tint;
  CurrentlyPlayingTrackArtist.Render();
  Renderer->DrawSprite(NowPlaying, nowPlayingOffset, tint);
  if (CurrentlyPlayingTrackId.has_value()) {
    auto currentTrackId = CurrentlyPlayingTrackId.value();
    auto targetCenter = MainItems->Children[currentTrackId]->Bounds.Center();
    auto targetPos = MainItems->Children[currentTrackId]->Bounds.GetPos() +
                     HighlightStarRelativePos;
    if (MainItems->RenderingBounds.ContainsPoint(targetCenter)) {
      Renderer->DrawSprite(HighlightStar, targetPos);
    }
  }
  DrawButtonPrompt();
}

void MusicMenu::Update(float dt) {
  if ((!GetFlag(SF_SOUNDMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_SOUNDMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() &&
      (ScrWork[SW_SYSMENUCT] == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    MainItems->Hide();
    CurrentlyPlayingTrackName.Hide();
    CurrentlyPlayingTrackArtist.Hide();
    State = Hidden;
  } else if (MenuTransition.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
    InputEnabled = true;
  }

  if (State != Hidden) {
    FromSystemMenuTransition.Update(dt);
    SelectAnimation.Update(dt);
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    if (State == Shown) {
      UpdateInput(dt);
    }
    TitleFade.Update(dt);
    NowPlayingAnimation.Update(dt);
    MainScrollbar.Update(dt);
    PlayModeRepeatClickArea.Update(dt);
    PlayModeAllClickArea.Update(dt);
    UpdateTitles();
    if (InputEnabled) MainItems->Update(dt);
    if (!CurrentlyPlayingTrackId.has_value()) return;
    if (PlaybackMode != MusicPlaybackMode::RepeatOne &&
        CurrentlyPlayingTrackId < 40 &&
        abs(Audio::Channels[Audio::AC_BGM0]->PositionInSeconds() -
            PreviousPosition) > 1.0f) {
      CurrentlyPlayingTrackId.reset();
      NowPlayingAnimation.StartOut();
      Audio::Channels[Audio::AC_BGM0]->Stop(2.0f);
    }
    if (Audio::Channels[Audio::AC_BGM0]->GetState() == Audio::ACS_Stopped) {
      std::optional<int> trackId;
      if (PlaybackMode == MusicPlaybackMode::One) {
        trackId = std::nullopt;
      } else {
        trackId = GetNextTrackId(CurrentlyPlayingTrackId.value() + 1);
        if (trackId == MusicTrackCount) {
          if (PlaybackMode == MusicPlaybackMode::RepeatPlaylist) {
            trackId = GetNextTrackId(0);
          } else if (PlaybackMode == MusicPlaybackMode::Playlist) {
            trackId = std::nullopt;
          }
        }
      }
      SwitchToTrack(trackId);
    }
    PreviousPosition = Audio::Channels[Audio::AC_BGM0]->PositionInSeconds();
  }
}

void MusicMenu::UpdateInput(float dt) {
  auto* prevFocus = CurrentlyFocusedElement;
  if (!MainScrollbar.IsScrollHeld()) {
    Menu::UpdateInput(dt);
  }
  if (State == Shown) {
    const float prevScrollPos = ScrollY;
    auto checkScrollBounds = [&]() {
      return !TrackListBounds.Contains(CurrentlyFocusedElement->Bounds);
    };

    if (CurrentlyFocusedElement != prevFocus && checkScrollBounds()) {
      if (CurrentlyFocusedElement == MainItems->GetFirstFocusableChild()) {
        ScrollY = 0;
      } else if (CurrentlyFocusedElement == MainItems->Children.back()) {
        ScrollY = MainScrollbar.EndValue;
      } else {
        float targetScroll = CurrentlyFocusedElement->Bounds.Y -
                             MainItems->GetFirstFocusableChild()->Bounds.Y;
        // if focused item is out of bounds, we scroll to it if focus has
        // changed
        if (targetScroll <= ScrollY) {
          ScrollY = targetScroll;
        } else {
          ScrollY = targetScroll - SelectableItemsPerPage * TrackOffset.y;
        }
        MainScrollbar.ClampValue();
      }
    }

    // imitate gamepad's d-pad/keyboard controls behavior, when scrolling out of
    // bounds
    if (Input::CurrentInputDevice != Input::Device::Mouse) {
      int currentLowerBound = (int)(ScrollY / TrackOffset.y);
      int currentUpperBound = currentLowerBound + SelectableItemsPerPage;
      auto btnId = static_cast<Widgets::CHLCC::TrackSelectButton*>(
                       CurrentlyFocusedElement)
                       ->Id;
      if (btnId == currentLowerBound && btnId != 0) {
        ScrollY -= TrackOffset.y;
      }

      if (btnId == currentUpperBound && btnId != MusicTrackCount - 1) {
        ScrollY += TrackOffset.y;
      }
    }

    MainItems->UpdateInput(dt);
    MainScrollbar.UpdateInput(dt);
    PlayModeRepeatClickArea.UpdateInput(dt);
    PlayModeAllClickArea.UpdateInput(dt);

    if (prevScrollPos != ScrollY) {
      float delta = prevScrollPos - ScrollY;
      if (std::fmod(std::abs(delta), TrackOffset.y) >
          std::numeric_limits<float>::epsilon()) {
        const float newDelta =
            std::round(delta / TrackOffset.y) * TrackOffset.y;
        ScrollY = prevScrollPos - newDelta;
        delta = newDelta;
      }
      glm::vec2 itemsPos = {0, -ScrollY};
      MainItems->MoveTo(itemsPos);
      for (auto child : MainItems->Children) {
        auto button = static_cast<Widgets::CHLCC::TrackSelectButton*>(child);
        button->MoveTracks(itemsPos);
      }
    }

    if (PADinputButtonWentDown & PAD1Y) {
      ++PlaybackMode;
      PlayModeAllSprite =
          isPlaylist(PlaybackMode) ? PlaymodeAllHighlight : PlaymodeAll;
      PlayModeRepeatSprite =
          isRepeat(PlaybackMode) ? PlaymodeRepeatHighlight : PlaymodeRepeat;
      UpdateLooping();
    }

    if (PADinputButtonWentDown & PAD1X) {
      SwitchToTrack(std::nullopt);
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

inline void MusicMenu::DrawButtonPrompt() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (MenuTransition.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (MenuTransition.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
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

void MusicMenu::SwitchToTrack(std::optional<int> id) {
  if (CurrentlyPlayingTrackId == id) {
    return;
  }

  CurrentlyPlayingTrackId = id;
  if (!id.has_value()) {
    NowPlayingAnimation.StartOut();
    Audio::Channels[Audio::AC_BGM0]->Stop(0.5f);
    return;
  }
  if (!NowPlayingAnimation.IsIn()) NowPlayingAnimation.StartIn();
  CurrentlyPlayingTrackName = Label(
      Vm::ScriptGetTextTableStrAddress(4, CurrentlyPlayingTrackId.value() * 3),
      NowPlayingPos + PlayingTrackOffset, 32, RendererOutlineMode::None, 0);
  CurrentlyPlayingTrackArtist =
      Label(Vm::ScriptGetTextTableStrAddress(
                4, CurrentlyPlayingTrackId.value() * 3 + 2),
            NowPlayingPos + PlayingTrackArtistOffset, 20,
            RendererOutlineMode::None, 0);
  PreviousPosition = 0.0f;
  Audio::Channels[Audio::AC_BGM0]->Play(
      "bgm", Playlist[id.value()],
      id >= 40 ? (PlaybackMode == MusicPlaybackMode::RepeatOne) : true, 0.5f);
}

inline int MusicMenu::GetNextTrackId(int id) {
  while (!SaveSystem::GetBgmFlag(Playlist[id])) {
    id += 1;
    if (id == MusicTrackCount) {
      if (PlaybackMode == MusicPlaybackMode::RepeatPlaylist) {
        id = 0;
      } else if (PlaybackMode == MusicPlaybackMode::Playlist) {
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