#include "musicmenu.h"

#include <numeric>
#include "../../ui/ui.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../video/videosystem.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../text.h"
#include "../../profile/dialogue.h"
#include "../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;

MusicTrackButton::MusicTrackButton(int id, int position, glm::vec2 pos)
    : Button(), Position(position) {
  Id = id;
  Bounds = RectF(pos.x, pos.y + MusicButtonTextYOffset, MusicButtonBounds.Width,
                 MusicButtonBounds.Height);
  IsLocked = !SaveSystem::GetBgmFlag(MusicBGMFlagIds[Id]);
  size_t trackTextIndex = 2 * Id;
  SetText(
      Vm::ScriptGetTextTableStrAddress(MusicStringTableId, trackTextIndex + 6),
      MusicTrackNameSize, RendererOutlineMode::None,
      {MusicButtonTextColor, MusicButtonTextOutlineColor});
  Bounds =
      RectF(pos.x, pos.y, MusicButtonBounds.Width, MusicButtonBounds.Height);
  HighlightSprite = MusicButtonHoverSprite;
  FocusedSprite = MusicButtonSelectSprite;
  HoverBounds = Bounds;
  for (int i = 0; i < Text.size(); i++) {
    Text[i].DestRect.X += MusicTrackNameOffsetX;
  }
  auto* const lockedSc3Text = Vm::ScriptGetTextTableStrAddress(
      MusicStringTableId, MusicStringLockedIndex);
  Vm::Sc3VmThread dummy;
  dummy.Ip = lockedSc3Text;
  TextLayoutPlainLine(&dummy, 6, LockedText, Profile::Dialogue::DialogueFont,
                      MusicTrackNameSize,
                      {MusicButtonTextColor, MusicButtonTextOutlineColor}, 1.0f,
                      glm::vec2(Bounds.X + MusicTrackNameOffsetX,
                                Bounds.Y + MusicButtonTextYOffset),
                      TextAlignment::Left);
  ArtistName = Widgets::Label(
      Vm::ScriptGetTextTableStrAddress(MusicStringTableId, trackTextIndex + 7),
      glm::vec2(Bounds.X + MusicTrackArtistOffsetX,
                Bounds.Y + MusicButtonTextYOffset),
      MusicTrackArtistSize, RendererOutlineMode::None,
      {MusicButtonTextColor, MusicButtonTextOutlineColor});
  TextLayoutPlainString(fmt::format("{:02}", position), NumberText,
                        Profile::Dialogue::DialogueFont, MusicTrackNameSize,
                        {0xfffffff, 0}, 1.0f,
                        glm::vec2(Bounds.X + MusicTrackNumberOffsetX,
                                  Bounds.Y + MusicButtonTextYOffset),
                        TextAlignment::Center);
}

void MusicTrackButton::Show() {
  Button::Show();
  ArtistName.Show();
  // todo check locked from bgm flags
}

void MusicTrackButton::Move(glm::vec2 relativePos) {
  const auto maxY = MusicButtonBounds.Height * MusicPlayIds.size();
  const float sum = Bounds.Y + relativePos.y;
  float newY = sum;
  if (relativePos.y > 0 && newY > maxY - MusicButtonBounds.Height) {
    newY -= maxY;
  } else if (relativePos.y < 0 && newY < -MusicButtonBounds.Height) {
    newY += maxY;
  }

  const float yDiff = (newY - Bounds.Y);

  const auto moveGlyphs = [&](std::span<ProcessedTextGlyph> glyphs,
                              glm::vec2 offset) {
    for (auto& glyph : glyphs) {
      glyph.DestRect.X += offset.x;
      glyph.DestRect.Y += offset.y;
    }
  };
  const auto move = [&](glm::vec2 offset) {
    Button::Move(offset);
    ArtistName.Move(offset);
    HoverBounds = Bounds;

    moveGlyphs(NumberText, offset);
    moveGlyphs(LockedText, offset);
  };
  move({relativePos.x, yDiff});
}

void MusicTrackButton::Update(float dt) {
  const int alpha = ((ScrWork[SW_SYSSUBMENUCT] * 32 - 768) * 224) >> 8;
  Tint = glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f);
  Button::Update(dt);
  if (HasFocus && !PrevFocusState)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
  if (PrevFocusState != HasFocus) {
    PrevFocusState = HasFocus;
  }
}

void MusicTrackButton::Render() {
  if (Enabled) {
    ArtistName.Tint = Tint;
    if (Selected) {
      Renderer->DrawSprite(
          MusicButtonPlayingSprite,
          MusicButtonPlayingDispOffset + glm::vec2(Bounds.X, Bounds.Y), Tint);
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X + 113, Bounds.Y),
                           Tint);
    } else {
      Renderer->DrawProcessedText(NumberText, Profile::Dialogue::DialogueFont,
                                  Tint.a);
    }
    if (HasFocus) {
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X + 113, Bounds.Y),
                           Tint);
    }
    if (IsLocked) {
      Renderer->DrawProcessedText(LockedText, Profile::Dialogue::DialogueFont,
                                  Tint.a);
    } else {
      Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont,
                                  Tint.a);
      ArtistName.Render();
    }
  }
}

MusicModeButton::MusicModeButton(
    Profile::CCLCC::LibraryMenu::MusicMenuPlayingMode& mode)
    : Widgets::Button(), PlayMode(mode) {
  OnClickHandler = [](Widgets::Button* btn) {
    auto* modeBtn = static_cast<MusicModeButton*>(btn);
    modeBtn->PlayMode = MusicMenuPlayingMode::_from_integral(
        (modeBtn->PlayMode._to_integral() + 1) % MusicMenuPlayingMode::_size());
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
  };
}

void MusicModeButton::Update(float dt) {
  Widgets::Button::Update(dt);
  NormalSprite = MusicPlayingModeSprites[PlayMode._to_integral()];
  FocusedSprite = NormalSprite;
  Bounds = MusicPlayingModeDisplayBounds[PlayMode._to_integral()];
}

MusicMenu::MusicMenu()
    : LibrarySubmenu(),
      DirectionButtonHoldHandler(
          MusicDirectionalHoldTime, MusicDirectionalFocusTimeInterval,
          Vm::Interface::PAD1UP | Vm::Interface::PAD1DOWN),
      ModeButton(PlayMode) {
  MainItems.RenderingBounds = MusicRenderingBounds;
  MainItems.HoverBounds = MusicHoverBounds;
  NowPlayingFadeAnimation.DurationIn = MusicNowPlayingNotificationFadeIn;
  NowPlayingFadeAnimation.DurationOut = MusicNowPlayingNotificationFadeOut;
}

void MusicMenu::Show() {
  Audio::Channels[Audio::AC_BGM0]->Stop(0.0f);
  LibrarySubmenu::Show();
}

void MusicMenu::Init() {
  const auto musicOnclick = [this](Widgets::Button* target) {
    auto* musicBtn = static_cast<MusicTrackButton*>(target);
    if (target->IsLocked || ModeButton.Hovered) return;
    if (CurrentlyPlayingBtn) CurrentlyPlayingBtn->Selected = false;
    PlayTrack(musicBtn->Id);
    if (PlayMode == +MusicMenuPlayingMode::Shuffle) {
      ResetShuffle();
    }
    musicBtn->Selected = true;
  };
  const float maxY = MusicPlayIds.size() * MusicButtonBounds.Height;

  // Small indie company please understand
  SaveSystem::SetBgmFlag(101, true);
  SaveSystem::SetBgmFlag(131, true);
  MainItems.Clear();
  for (size_t pos = 1; pos <= MusicPlayIds.size(); ++pos) {
    const size_t i = (pos + MusicPlayIds.size() - 1) % MusicPlayIds.size();
    const float btnY =
        fmod(MusicButtonBounds.Y + MusicButtonBounds.Height * pos +
                 MusicButtonBounds.Height * 8,
             maxY);
    const glm::vec2 btnPos = {MusicButtonBounds.X, btnY};
    const auto musicItem = new MusicTrackButton((int)i, (int)pos, btnPos);

    musicItem->OnClickHandler = musicOnclick;
    MainItems.Add(musicItem, FDIR_DOWN);
  }
  ResetShuffle();
}

void MusicMenu::StopMusic(bool playTitle) {
  if (CurrentlyPlayingBtn) {
    Audio::Channels[Audio::AC_BGM0]->Stop(0.0f);
    CurrentlyPlayingBtn->Selected = false;
    CurrentlyPlayingBtn = nullptr;
    NowPlayingFadeAnimation.StartOut();
    NowPlayingTrackName.ClearText();
    if (playTitle)
      Audio::Channels[Audio::AC_BGM0]->Play("bgm", 101, true, 0.0f);
  }
}

void MusicMenu::Update(float dt) {
  const auto getNextUnlockedTrack =
      [this](size_t current) -> std::optional<size_t> {
    size_t next = (current + 1) % MusicPlayIds.size();
    while (!SaveSystem::GetBgmFlag(MusicBGMFlagIds[next]) && next != current) {
      next = (next + 1) % MusicPlayIds.size();
    }
    return next == current ? std::nullopt : std::make_optional(next);
  };
  if (!IsFocused) {
    StopMusic(true);
  }
  if (IsFocused && CurrentlyPlayingBtn &&
      Audio::Channels[Audio::AC_BGM0]->GetState() ==
          Audio::AudioChannelState::ACS_Stopped) {
    switch (PlayMode) {
      case MusicMenuPlayingMode::RepeatOne:
        PlayTrack(CurrentlyPlayingBtn->Id);
        break;
      case MusicMenuPlayingMode::PlayAll: {
        if (CurrentlyPlayingBtn->Id != MusicPlayIds.size() - 1) {
          auto nextTrack = getNextUnlockedTrack(CurrentlyPlayingBtn->Id);
          if (nextTrack) {
            PlayTrack(*nextTrack);
            break;
          }
        }
        StopMusic();
      } break;
      case MusicMenuPlayingMode::RepeatAll: {
        auto nextTrack = getNextUnlockedTrack(CurrentlyPlayingBtn->Id);
        PlayTrack(nextTrack.value_or(CurrentlyPlayingBtn->Id));
      } break;
      case MusicMenuPlayingMode::Shuffle: {
        if (ShuffleTrackIndices.empty()) {
          ResetShuffle();
        }
        PlayTrack(ShuffleTrackIndices.back());
        ShuffleTrackIndices.pop_back();
      } break;
    }
  }
  LibrarySubmenu::Update(dt);
  ModeButton.HasFocus = ModeButton.Hovered;
  BGWidget.Update(dt);
  NowPlayingFadeAnimation.Update(dt);
  NowPlayingTrackName.Update(dt);
  ModeButton.Update(dt);
  const int alpha = ((ScrWork[SW_SYSSUBMENUCT] * 32 - 768) * 224) >> 8;
  const auto tint =
      glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f * FadeAnimation.Progress);
  MainItems.Tint = tint;
  BGWidget.Tint = tint;
  NowPlayingTrackName.Tint = tint;
  ModeButton.Tint = tint;
}

void MusicMenu::Hide() {
  if (CurrentlyFocusedElement) CurrentlyFocusedElement = nullptr;
  StopMusic(true);
  Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0);
  MainItems.MoveTo(glm::vec2(0, 0));
  LibrarySubmenu::Hide();
}

void MusicMenu::UpdateInput(float dt) {
  using namespace Vm::Interface;
  if (State == Shown && IsFocused) {
    if (PADinputButtonWentDown & PADcustom[17]) {
      StopMusic();
    }

    if (PADinputButtonWentDown & PADcustom[18]) {
      PlayMode = MusicMenuPlayingMode::_from_integral(
          (PlayMode._to_integral() + 1) % MusicMenuPlayingMode::_size());
    }
    ModeButton.UpdateInput();

    const uint32_t btnUp = PADcustom[0];
    const uint32_t btnDown = PADcustom[1];
    const bool upScroll = Input::MouseWheelDeltaY > 0;
    const bool downScroll = Input::MouseWheelDeltaY < 0;

    DirectionButtonHoldHandler.Update(dt);
    const int directionShouldFire = DirectionButtonHoldHandler.ShouldFire();
    const bool directionMovement =
        (bool)(directionShouldFire & btnUp || upScroll) ^
        (bool)(directionShouldFire & btnDown || downScroll);

    if (directionMovement) {
      const bool dirDown = directionShouldFire & btnDown || downScroll;
      QueuedMove =
          (dirDown ? FocusDirection::FDIR_DOWN : FocusDirection::FDIR_UP);
    } else if (TurboMoved) {
      QueuedMove.reset();
    }

    if (QueuedMove.has_value() &&
        (MainItems.MoveAnimation.State != +AnimationState::Playing &&
         BGWidget.MoveAnimation.State != +AnimationState::Playing)) {
      float deltaY = 0;
      const bool dirDown = *QueuedMove == FocusDirection::FDIR_DOWN;
      deltaY += dirDown ? MusicButtonBounds.Height : -MusicButtonBounds.Height;
      TurboMoved = DirectionButtonHoldHandler.IsTurbo || upScroll || downScroll;
      const float animationSpeed =
          TurboMoved ? MusicDirectionalFocusTimeInterval : 0.3f;

      MainItems.Move({0.0f, -deltaY}, animationSpeed);
      BGWidget.Move({0.0f, -deltaY}, animationSpeed);
      if (dirDown)
        AdvanceFocus(FocusDirection::FDIR_DOWN);
      else
        AdvanceFocus(FocusDirection::FDIR_UP);
      QueuedMove.reset();
    }
  }
}

void MusicBGs::Move(glm::vec2 relativePos) {
  const auto maxY = MusicItemsBackgroundRepeatHeight;
  const float sum = Bounds.Y + relativePos.y;
  float newY = sum;
  if (relativePos.y > 0 && newY > maxY - MusicButtonBounds.Height) {
    newY -= maxY;
  } else if (relativePos.y < 0 && newY < -MusicButtonBounds.Height) {
    newY += maxY;
  }

  const float yDiff = (newY - Bounds.Y);
  Widget::Move({relativePos.x, yDiff});
}

void MusicBGs::Render() {
  glm::vec2 backgroundPos = MusicItemsBackgroundPosition;
  glm::vec2 topSplitPos{
      MusicRenderingBounds.X,
      MusicRenderingBounds.Y + Bounds.Y - MusicItemsBackgroundRepeatHeight};
  glm::vec2 botSplitPos(MusicRenderingBounds.X,
                        MusicRenderingBounds.Y + Bounds.Y);
  Renderer->DrawSprite(MusicItemsBackgroundSprite, topSplitPos, Tint);
  Renderer->DrawSprite(MusicItemsBackgroundSprite, botSplitPos, Tint);

  Renderer->DrawSprite(MusicItemsOverlaySprite, topSplitPos, Tint);
  Renderer->DrawSprite(MusicItemsOverlaySprite, botSplitPos, Tint);
}

void MusicMenu::Render() {
  if (State != Hidden) {
    BGWidget.Render();
    LibrarySubmenu::Render();
    Renderer->DrawSprite(
        MusicNowPlayingNotificationSprite, MusicNowPlayingNotificationPos,
        glm::vec4{glm::vec3{1.0f}, NowPlayingFadeAnimation.Progress});
    NowPlayingTrackName.Render();
    ModeButton.Render();
  }
}

void MusicMenu::PlayTrack(size_t index) {
  if (index >= MusicPlayIds.size()) return;
  if (CurrentlyPlayingBtn) {
    CurrentlyPlayingBtn->Selected = false;
    CurrentlyPlayingBtn = nullptr;
  }
  CurrentlyPlayingBtn =
      static_cast<MusicTrackButton*>(MainItems.Children[index]);
  CurrentlyPlayingBtn->Selected = true;
  NowPlayingFadeAnimation.StartIn();

  const glm::vec2 playingNamePos =
      MusicNowPlayingNotificationPos + MusicNowPlayingNotificationTrackOffset;
  NowPlayingTrackName.Bounds.X = playingNamePos.x;
  NowPlayingTrackName.Bounds.Y = playingNamePos.y;
  NowPlayingTrackName.SetText(
      Vm::ScriptGetTextTableStrAddress(MusicStringTableId, 2 * index + 6),
      MusicNowPlayingNotificationTrackFontSize, RendererOutlineMode::None,
      {MusicNowPlayingTextColor, MusicNowPlayingTextOutlineColor});
  NowPlayingTrackName.Show();
  Audio::Channels[Audio::AC_BGM0]->Play("bgm", MusicPlayIds[index], false,
                                        0.0f);
}

void MusicMenu::ResetShuffle() {
  static std::random_device randomDevice{};
  if (PlayMode == +MusicMenuPlayingMode::Shuffle) {
    ShuffleTrackIndices.clear();
    for (size_t i = 0; i < MusicPlayIds.size(); ++i) {
      if (static_cast<MusicTrackButton*>(MainItems.Children[i])->IsLocked)
        continue;
      ShuffleTrackIndices.push_back(i);
    }
    std::shuffle(ShuffleTrackIndices.begin(), ShuffleTrackIndices.end(),
                 std::mt19937{randomDevice()});
  }
}
}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto