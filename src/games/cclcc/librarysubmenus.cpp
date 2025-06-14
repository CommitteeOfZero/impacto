#include "../../ui/ui.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../video/videosystem.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../text.h"
#include "../../profile/dialogue.h"
#include "librarysubmenus.h"

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

void MusicTrackButton::Update(float dt) { Button::Update(dt); }

void MusicTrackButton::Render() {
  if (Enabled) {
    Renderer->DrawProcessedText(NumberText, Profile::Dialogue::DialogueFont);
    if (IsLocked) {
      Renderer->DrawProcessedText(LockedText, Profile::Dialogue::DialogueFont);
    } else {
      Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont);
      ArtistName.Render();
    }
  }
}

LibrarySubmenu::LibrarySubmenu() {
  FadeAnimation.DurationIn = SubMenuFadeInDuration;
  FadeAnimation.DurationOut = SubMenuFadeOutDuration;
}

void LibrarySubmenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void LibrarySubmenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems.Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void LibrarySubmenu::Update(float dt) {
  FadeAnimation.Update(dt);
  MainItems.Update(dt);
  UpdateInput();
  if (CurrentlyFocusedElement) {
    CurrentlyFocusedElement->Update(dt);
  }
  if (State == Showing && FadeAnimation.IsIn()) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.IsOut()) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }
}

void LibrarySubmenu::Render() {
  if (State != Hidden) {
    MainItems.Tint = glm::vec4(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    MainItems.Render();
  }
}

MovieMenu::MovieMenu() : LibrarySubmenu() {
  for (int i = 0; i < MovieDiskSprites.size(); ++i) {
    const auto& diskSprite = MovieDiskSprites[i];
    const auto& diskHighlightSprite = MovieDiskHighlightSprites[i];
    auto movieOnclick = [](Widgets::Button* target) {
      ScrWork[SW_MOVIEMODE_CUR] = MovieDiskPlayIds[target->Id];
      LibraryMenuPtr->AllowsScriptInput = true;
    };
    auto disk =
        new Widgets::Button(i, diskSprite, Sprite(), diskHighlightSprite,
                            MovieDiskDisplayPositions[i]);
    disk->OnClickHandler = movieOnclick;
    MainItems.Add(disk, FDIR_RIGHT);
  }
}

void MovieMenu::Show() {
  if (State != Shown) {
    MainItems.Children[0]->Enabled = true;
    MainItems.Children[1]->Enabled = GetFlag(SF_CLR_TRUE_CCLCC);
    MainItems.Children[2]->Enabled = [&]() {
      for (int i = 1; i < 10; ++i) {
        if (GetFlag(SF_CLR_END1 + i)) return true;
      }
      return false;
    }();
    MainItems.Children[3]->Enabled = GetFlag(SF_CLR_END1);
    MainItems.Show();
  }
  LibrarySubmenu::Show();
}

MusicMenu::MusicMenu() : LibrarySubmenu() {}

void MusicMenu::Show() {
  if (State == MenuState::Hidden) {
    const int maxY = MusicPlayIds.size() * MusicButtonBounds.Height;
    for (int pos = 1; pos <= MusicPlayIds.size(); ++pos) {
      int i = (pos + MusicPlayIds.size() - 1) % MusicPlayIds.size();
      auto musicOnclick = [this](Widgets::Button* target) {
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
  if (State == Shown) {
    auto lastPageY = PageY;
    float deltaY = 0;
    deltaY += (Vm::Interface::PADinputButtonIsDown & Vm::Interface::PAD1DOWN)
                  ? 10.0f
                  : 0.0f;
    deltaY -= (Vm::Interface::PADinputButtonIsDown & Vm::Interface::PAD1UP)
                  ? 10.0f
                  : 0.0f;
    MainItems.Move(glm::vec2(0, -deltaY));
    PageY += deltaY;
    if (PageY != lastPageY) {
      auto maxPageY = MusicButtonBounds.Height * MusicPlayIds.size();
      if (PageY < lastPageY && PageY < MusicButtonBounds.Height)
        PageY += maxPageY - MusicButtonBounds.Height;
      else if (PageY > lastPageY &&
               PageY > maxPageY - MusicButtonBounds.Height) {
        PageY -= maxPageY - MusicButtonBounds.Height;
      }
    }
  } else if (State == Hidden && !MainItems.Children.empty()) {
    PageY = 0;
    MainItems.Clear();
    MainItems.MoveTo(glm::vec2(0, 0));
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