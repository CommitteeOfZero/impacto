#include "musicmenu.h"

#include "../../ui/ui.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../video/videosystem.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../text.h"
#include "../../profile/dialogue.h"

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
  auto* lockedSc3Text = Vm::ScriptGetTextTableStrAddress(
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
  TextLayoutPlainString(
      fmt::format("{}", position), NumberText, Profile::Dialogue::DialogueFont,
      MusicTrackNameSize, {0xfffffff, 0}, 1.0f,
      glm::vec2(Bounds.X + 80.0f, Bounds.Y + MusicButtonTextYOffset),
      TextAlignment::Center);
}

void MusicTrackButton::Show() {
  Button::Show();
  ArtistName.Show();
  // todo check locked from bgm flags
}

void MusicTrackButton::Move(glm::vec2 relativePos) {
  auto maxY = MusicButtonBounds.Height * MusicPlayIds.size();
  float sum = Bounds.Y + relativePos.y;
  float newY = sum;
  if (relativePos.y > 0 && newY > maxY - MusicButtonBounds.Height) {
    newY -= maxY;
  } else if (relativePos.y < 0 && newY < -MusicButtonBounds.Height) {
    newY += maxY;
  }

  float yDiff = (newY - Bounds.Y);

  auto moveGlyphs = [&](std::span<ProcessedTextGlyph> glyphs,
                        glm::vec2 offset) {
    for (auto& glyph : glyphs) {
      glyph.DestRect.X += offset.x;
      glyph.DestRect.Y += offset.y;
    }
  };
  auto move = [&](glm::vec2 offset) {
    Button::Move(offset);
    ArtistName.Move(offset);
    HoverBounds = Bounds;

    moveGlyphs(NumberText, offset);
    moveGlyphs(LockedText, offset);
  };
  move({relativePos.x, yDiff});
}

void MusicTrackButton::Update(float dt) {
  int alpha = ((ScrWork[SW_SYSSUBMENUCT] * 32 - 768) * 224) >> 8;
  Tint = glm::vec4(1.0f, 1.0f, 1.0f, alpha / 255.0f);
  Button::Update(dt);
}

void MusicTrackButton::Render() {
  if (Enabled) {
    if (Selected) {
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X + 112, Bounds.Y),
                           Tint);
    } else if (HasFocus) {
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X + 112, Bounds.Y),
                           Tint);
    }
    Renderer->DrawProcessedText(NumberText, Profile::Dialogue::DialogueFont);
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
    const int maxY = MusicPlayIds.size() * MusicButtonBounds.Height;
    for (int pos = 1; pos <= MusicPlayIds.size(); ++pos) {
      int i = (pos + MusicPlayIds.size() - 1) % MusicPlayIds.size();
      auto musicOnclick = [this](Widgets::Button* target) {
        auto* musicBtn = static_cast<MusicTrackButton*>(target);
        if (CurrentlyPlayingBtn) CurrentlyPlayingBtn->Selected = false;
        musicBtn->Selected = true;
        CurrentlyPlayingBtn = musicBtn;
        Audio::Channels[Audio::AC_BGM0]->Play(
            "bgm", MusicPlayIds[target->Id],
            PlayMode == MusicPlayMode::RepeatOne, 0.0f);
      };
      float btnY = fmod(MusicButtonBounds.Y + MusicButtonBounds.Height * pos +
                            MusicButtonBounds.Height * 8,
                        maxY);
      glm::vec2 btnPos = {MusicButtonBounds.X, btnY};
      auto musicItem = new MusicTrackButton(i, pos, btnPos);

      musicItem->OnClickHandler = musicOnclick;
      MainItems.Add(musicItem, FDIR_DOWN);
    }
  }

  LibrarySubmenu::Show();
  MainItems.Show();
}

void MusicMenu::Update(float dt) {
  LibrarySubmenu::Update(dt);
  if (State == Hidden && !MainItems.Children.empty()) {
    PageY = 0;
    MainItems.Clear();
    MainItems.MoveTo(glm::vec2(0, 0));
  }
}

void MusicMenu::Hide() {
  if (CurrentlyPlayingBtn) {
    CurrentlyPlayingBtn->Selected = false;
    CurrentlyPlayingBtn = nullptr;
  }
  if (CurrentlyFocusedElement) CurrentlyFocusedElement = nullptr;
  Audio::Channels[Audio::AC_BGM0]->Play("bgm", 101, true, 0.0f);

  MainItems.Clear();
  MainItems.MoveTo(glm::vec2(0, 0));
  LibrarySubmenu::Hide();
}

void MusicMenu::UpdateInput(float dt) {
  using namespace Vm::Interface;
  if (State == Shown) {
    auto maxPageY = MusicButtonBounds.Height * MusicPlayIds.size();

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
      if (directionShouldFire & btnDown || downScroll) {
        AdvanceFocus(FDIR_DOWN);
        deltaY += MusicButtonBounds.Height;
      } else {
        AdvanceFocus(FDIR_UP);
        deltaY -= MusicButtonBounds.Height;
      }
      MainItems.Move(glm::vec2(0, -deltaY));
      auto lastPageY = PageY;
      PageY += deltaY;
      if (PageY != lastPageY) {
        if (PageY < lastPageY && PageY < MusicButtonBounds.Height)
          PageY += maxPageY - MusicButtonBounds.Height;
        else if (PageY > lastPageY &&
                 PageY > maxPageY - MusicButtonBounds.Height) {
          PageY -= maxPageY - MusicButtonBounds.Height;
        }
      }
    }
  }
}

void MusicMenu::Render() {
  if (State != Hidden) {
    float backgroundY =
        fmod(MusicRenderingBounds.Y - PageY - MusicButtonBounds.Height,
             MusicItemsBackgroundRepeatHeight);
    glm::vec2 backgroundPos = MusicItemsBackgroundPosition;
    Renderer->DrawSprite(MusicItemsBackgroundSprite,
                         glm::vec2(MusicRenderingBounds.X, backgroundY));
    Renderer->DrawSprite(
        MusicItemsBackgroundSprite,
        glm::vec2(MusicRenderingBounds.X,
                  backgroundY + MusicItemsBackgroundRepeatHeight));

    Renderer->DrawSprite(MusicItemsOverlaySprite,
                         glm::vec2(MusicRenderingBounds.X, backgroundY));
    Renderer->DrawSprite(
        MusicItemsOverlaySprite,
        glm::vec2(MusicRenderingBounds.X,
                  backgroundY + MusicItemsBackgroundRepeatHeight));
  }
  LibrarySubmenu::Render();
}
}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto