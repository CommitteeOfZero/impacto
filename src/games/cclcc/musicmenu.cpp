#include "musicmenu.h"

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
      MusicTrackNameSize, RendererOutlineMode::None, {0x4f4f4b, 0x0});
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
                      MusicTrackNameSize, {0x4f4f4b, 0x0}, 1.0f,
                      glm::vec2(Bounds.X + MusicTrackNameOffsetX,
                                Bounds.Y + MusicButtonTextYOffset),
                      TextAlignment::Left);
  ArtistName = Widgets::Label(
      Vm::ScriptGetTextTableStrAddress(MusicStringTableId, trackTextIndex + 7),
      glm::vec2(Bounds.X + MusicTrackArtistOffsetX,
                Bounds.Y + MusicButtonTextYOffset),
      MusicTrackArtistSize, RendererOutlineMode::None, {0x4f4f4b, 0x0});
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
    if (Selected) {
      Renderer->DrawSprite(
          MusicButtonPlayingSprite,
          MusicButtonPlayingDispOffset + glm::vec2(Bounds.X, Bounds.Y), Tint);
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X + 113, Bounds.Y),
                           Tint);
    } else {
      Renderer->DrawProcessedText(NumberText, Profile::Dialogue::DialogueFont);
    }
    if (HasFocus) {
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X + 113, Bounds.Y),
                           Tint);
    }
    if (IsLocked) {
      Renderer->DrawProcessedText(LockedText, Profile::Dialogue::DialogueFont);
    } else {
      Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont);
      ArtistName.Render();
    }
  }
}

MusicMenu::MusicMenu()
    : LibrarySubmenu(),
      DirectionButtonHoldHandler(
          MusicDirectionalHoldTime, MusicDirectionalFocusTimeInterval,
          Vm::Interface::PAD1UP | Vm::Interface::PAD1DOWN) {
  MainItems.RenderingBounds = MusicRenderingBounds;
  MainItems.HoverBounds = MusicHoverBounds;
}

void MusicMenu::Show() {
  if (State == MenuState::Hidden) {
    Audio::Channels[Audio::AC_BGM0]->Stop(0.0f);
    const float maxY = MusicPlayIds.size() * MusicButtonBounds.Height;
    const auto musicOnclick = [this](Widgets::Button* target) {
      auto* musicBtn = static_cast<MusicTrackButton*>(target);
      if (target->IsLocked) return;
      if (CurrentlyPlayingBtn) CurrentlyPlayingBtn->Selected = false;
      musicBtn->Selected = true;
      CurrentlyPlayingBtn = musicBtn;
      Audio::Channels[Audio::AC_BGM0]->Play(
          "bgm", MusicPlayIds[target->Id], PlayMode == MusicPlayMode::RepeatOne,
          0.0f);
    };
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
  }

  LibrarySubmenu::Show();
  MainItems.Show();
}

void MusicMenu::Update(float dt) {
  LibrarySubmenu::Update(dt);
  BGWidget.Update(dt);
  if (State == Hidden && !MainItems.Children.empty()) {
    PageY = 0;
    MainItems.Clear();
    MainItems.MoveTo(glm::vec2(0, 0));
  }
  const int alpha = ((ScrWork[SW_SYSSUBMENUCT] * 32 - 768) * 224) >> 8;
  const auto tint =
      glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f * FadeAnimation.Progress);
  MainItems.Tint = tint;
  BGWidget.Tint = tint;
}

void MusicMenu::Hide() {
  if (CurrentlyPlayingBtn) {
    CurrentlyPlayingBtn->Selected = false;
    CurrentlyPlayingBtn = nullptr;
  }
  if (CurrentlyFocusedElement) CurrentlyFocusedElement = nullptr;

  Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0);
  Audio::Channels[Audio::AC_BGM0]->Play("bgm", 101, true, 0.0f);

  MainItems.Clear();
  MainItems.MoveTo(glm::vec2(0, 0));
  LibrarySubmenu::Hide();
}

void MusicMenu::UpdateInput(float dt) {
  using namespace Vm::Interface;
  if (State == Shown) {
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
      float deltaY = 0;
      bool dirDown = directionShouldFire & btnDown || downScroll;
      deltaY += dirDown ? MusicButtonBounds.Height : -MusicButtonBounds.Height;
      const float animationSpeed = DirectionButtonHoldHandler.IsTurbo
                                       ? MusicDirectionalFocusTimeInterval
                                       : 0.4f;
      if (MainItems.MoveAnimation.State == +AnimationState::Playing ||
          BGWidget.MoveAnimation.State == +AnimationState::Playing) {
        MainItems.MoveAnimation.Stop();
        BGWidget.MoveAnimation.Stop();
        MainItems.MoveTo(MainItems.MoveTarget);
        BGWidget.MoveTo(BGWidget.MoveTarget);
      } else {
        MainItems.Move({0.0f, -deltaY}, animationSpeed);
        BGWidget.Move({0.0f, -deltaY}, animationSpeed);
        if (dirDown)
          AdvanceFocus(FocusDirection::FDIR_DOWN);
        else
          AdvanceFocus(FocusDirection::FDIR_UP);
      }
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
  }
  LibrarySubmenu::Render();
}
}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto