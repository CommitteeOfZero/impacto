#include "librarymenu.h"

#include <algorithm>
#include <cmath>

#include <fmt/format.h>

#include "fullwidth.h"
#include "../../profile/games/sg0/librarymenu.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/dialogue.h"
#include "../../profile/game.h"
#include "../../profile/scriptvars.h"
#include "../../data/savesystem.h"
#include "../../background2d.h"
#include "../../renderer/renderer.h"
#include "../../text/text.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../util.h"
#include "../../inputsystem.h"
#include "../../vm/vm.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"
#include "../../audio/audiochannel.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SG0::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

namespace {
constexpr int SubMenuCg = 8;
constexpr int SubMenuMovie = 9;
constexpr int SubMenuSound = 10;
constexpr int NoMovie = 255;
constexpr float ZoomStep = 1.25f;

int SubMenuFor(int library) {
  return library == 0 ? SubMenuCg : library == 1 ? SubMenuSound : SubMenuMovie;
}

void PlaySe(int id) {
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", id, false, 0);
}

std::vector<ProcessedTextGlyph> LayoutSystemString(int strNum, float size) {
  const uint32_t addr = Vm::ScriptGetStrAddress(SystemScriptBuffer, strNum);
  Vm::Sc3Stream stream(&Vm::ScriptBuffers[SystemScriptBuffer][addr]);
  return TextLayoutPlainLine(stream, 255, *Profile::Dialogue::DialogueFont,
                             size, {TextColor, 0}, 1.0f, glm::vec2(0.0f),
                             TextAlignment::Left);
}

std::vector<ProcessedTextGlyph> LayoutAscii(std::string_view text, float size) {
  return TextLayoutPlainString(
      FullWidth(text), *Profile::Dialogue::DialogueFont, size, {TextColor, 0},
      1.0f, glm::vec2(0.0f), TextAlignment::Left);
}

// Draws glyphs laid out at the origin at pos, recoloured.
void DrawText(std::span<const ProcessedTextGlyph> text, glm::vec2 pos,
              float opacity, uint32_t color) {
  std::vector<ProcessedTextGlyph> moved(text.begin(), text.end());
  for (ProcessedTextGlyph& glyph : moved) {
    glyph.Move(pos);
    glyph.Colors = {color, 0};
  }
  Profile::Dialogue::DialogueFont->DrawProcessedText(
      moved, opacity, opacity, Fonts::RendererOutlineMode::None);
}

std::string FormatTime(float seconds) {
  const int total = std::max(0, static_cast<int>(seconds));
  return fmt::format("{}:{:02}", total / 60, total % 60);
}

Background2D* CgBackground() {
  return Backgrounds2D[ScrWork[SW_BG1SURF + GetBufferId(CgLoadBuffer)]];
}
}  // namespace

void LibraryMenu::Init() {
  Active = true;
  ScrWork[SW_MOVIEMODE_CUR] = NoMovie;
  SetFlag(SF_ALBUMEND, false);
  SetFlag(SF_ALBUMLOAD, false);
  SetFlag(SF_ALBUMLOAD_COMPLETE, false);
  ViewerOpen = false;
  PlayingTrack = -1;
  switch (ScrWork[SW_SYSSUBMENUNO]) {
    case SubMenuSound:
      Current = Library::Sound;
      break;
    case SubMenuMovie:
      Current = Library::Movie;
      break;
    default:
      Current = Library::Cg;
      break;
  }
  if (!TextsLaidOut) LayoutTexts();
}

void LibraryMenu::LayoutTexts() {
  const int tracks = static_cast<int>(TrackBgmIds.size());
  const float titleWidth = ListArtistX - ListTitleX - 24.0f;
  for (int i = 0; i < tracks; i++) {
    TrackTitles.push_back(
        LayoutSystemString(TrackStringBase + 2 * i, ListFontSize));
    SquishText(TrackTitles.back(), titleWidth, 0.0f);
    TrackArtists.push_back(
        LayoutSystemString(TrackStringBase + 2 * i + 1, ListFontSize));
  }
  LockedTrackTitle = LayoutSystemString(LockedTrackString, ListFontSize);

  const float movieWidth = MovieCellPitch.x - 24.0f;
  for (size_t i = 0; i < MovieIds.size(); i++) {
    MovieTitles.push_back(LayoutSystemString(
        MovieStringBase + 2 * static_cast<int>(i), MovieFontSize));
    SquishText(MovieTitles.back(), movieWidth, 0.0f);
    MovieArtists.push_back(LayoutSystemString(
        MovieStringBase + 2 * static_cast<int>(i) + 1, MovieFontSize * 0.8f));
  }
  TextsLaidOut = true;
}

void LibraryMenu::Show() {
  if (State == Shown || State == Showing) return;
  State = Showing;
  if (UI::FocusedMenu != nullptr && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
}

void LibraryMenu::Hide() {
  if (State == Hidden || State == Hiding) return;
  State = Hiding;
  IsFocused = false;
  if (ViewerOpen) CloseViewer();
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
    LastFocusedMenu = nullptr;
  } else {
    UI::FocusedMenu = nullptr;
  }
}

bool LibraryMenu::AllEndingsCleared() const {
  return std::ranges::all_of(EndingFlags,
                             [](int flag) { return GetFlag(flag); });
}

int LibraryMenu::CgCount() const {
  return static_cast<int>(ThumbnailSprites.size());
}

bool LibraryMenu::IsCgVariationUnlocked(int entry, int variation) const {
  if (Profile::SaveSystem::AlbumEvData[entry][variation] == 0xFFFF)
    return false;
  if (std::ranges::find(AllClearCgs, entry) != AllClearCgs.end()) {
    return AllEndingsCleared();
  }
  return SaveSystem::GetEVVariationIsUnlocked(entry, variation);
}

bool LibraryMenu::IsCgUnlocked(int entry) const {
  for (int v = 0; v < Profile::SaveSystem::MaxAlbumSubEntries; v++) {
    if (Profile::SaveSystem::AlbumEvData[entry][v] == 0xFFFF) break;
    if (IsCgVariationUnlocked(entry, v)) return true;
  }
  return false;
}

bool LibraryMenu::IsMovieUnlocked(int movie) const {
  const std::vector<int>& lines = MovieUnlockLines[movie];
  for (size_t i = 0; i + 1 < lines.size(); i += 2) {
    if (SaveSystem::IsLineRead(lines[i], lines[i + 1])) return true;
  }
  return false;
}

bool LibraryMenu::IsTrackUnlocked(int track) const {
  if (SaveSystem::GetBgmFlag(TrackBgmIds[track])) return true;
  if (TrackBgmAltIds[track] >= 0 &&
      SaveSystem::GetBgmFlag(TrackBgmAltIds[track]))
    return true;
  return TrackMovieUnlocks[track] >= 0 &&
         IsMovieUnlocked(TrackMovieUnlocks[track]);
}

void LibraryMenu::Update(float dt) {
  Time += dt;
  const int subMenu = ScrWork[SW_SYSSUBMENUNO];
  const bool ours = subMenu == SubMenuCg || subMenu == SubMenuMovie ||
                    subMenu == SubMenuSound;
  const int count = Active && ours ? ScrWork[SW_SYSSUBMENUCT] : 0;
  if (count > 0 && (State == Hidden || State == Hiding) &&
      !GetFlag(SF_ALBUMEND)) {
    Show();
  } else if ((State == Shown || State == Showing) &&
             (count < LastSubMenuCount || count == 0 || GetFlag(SF_ALBUMEND))) {
    Hide();
  }
  LastSubMenuCount = count;

  if (State == Showing && count >= CountMax) {
    State = Shown;
    IsFocused = UI::FocusedMenu == this;
  } else if (State == Hiding && count <= 0) {
    State = Hidden;
    Active = false;
  }

  if (ViewerOpen && !ViewerLoaded && GetFlag(SF_ALBUMLOAD_COMPLETE)) {
    const Background2D* bg = CgBackground();
    if (bg != nullptr && bg->Status == LoadStatus::Loaded) {
      const glm::vec2 screen(Profile::Game::DesignWidth,
                             Profile::Game::DesignHeight);
      ViewerSize =
          glm::vec2(bg->BgSprite.ScaledWidth(), bg->BgSprite.ScaledHeight());
      ViewerCoverScale =
          std::max(screen.x / ViewerSize.x, screen.y / ViewerSize.y);
      ViewerContainScale =
          std::min(screen.x / ViewerSize.x, screen.y / ViewerSize.y);
      ViewerScale = ViewerCoverScale;
      ViewerPan =
          glm::vec2((screen.x - ViewerSize.x * ViewerScale) / 2.0f, 0.0f);
      ClampViewerPan();
      ViewerLoaded = true;
    }
  }

  if (Active) UpdatePlayback();
  if (State == Shown && MainCalled) UpdateInput(dt);
  MainCalled = false;
}

void LibraryMenu::SetLibrary(Library library) {
  if (library == Current) return;
  Current = library;
  ScrWork[SW_SYSSUBMENUNO] = SubMenuFor(static_cast<int>(library));
  PlaySe(1);
}

void LibraryMenu::Back() {
  SetFlag(SF_ALBUMEND, true);
  PlayingTrack = -1;
  PlaySe(3);
}

void LibraryMenu::UpdateInput(float dt) {
  if (!IsFocused || GetFlag(SF_ALBUMEND) ||
      ScrWork[SW_MOVIEMODE_CUR] != NoMovie) {
    return;
  }
  if (ViewerOpen) {
    UpdateViewerInput(dt);
    return;
  }
  if (PADinputButtonWentDown & PAD1B ||
      Input::MouseButtonWentDown[SDL_BUTTON_RIGHT]) {
    Back();
    return;
  }
  const int library = static_cast<int>(Current);
  if (PADinputButtonWentDown & PAD1L1) {
    SetLibrary(
        static_cast<Library>((library + LibraryCount - 1) % LibraryCount));
    return;
  }
  if (PADinputButtonWentDown & PAD1R1) {
    SetLibrary(static_cast<Library>((library + 1) % LibraryCount));
    return;
  }
  UpdateTabInput();
  switch (Current) {
    case Library::Cg:
      UpdateCgInput(dt);
      break;
    case Library::Sound:
      UpdateSoundInput(dt);
      break;
    case Library::Movie:
      UpdateMovieInput(dt);
      break;
  }
}

void LibraryMenu::UpdateTabInput() {
  if (!Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) return;
  // Tab 0 is CLEAR LIST, a separate EXTRA screen.
  for (int tab = 1; tab < TabCount; tab++) {
    const RectF bounds(TabPos.x, TabPos.y + tab * TabPitch,
                       TabSprites[tab].ScaledWidth(),
                       TabSprites[tab].ScaledHeight());
    if (bounds.ContainsPoint(Input::CurMousePos)) {
      SetLibrary(static_cast<Library>(tab - 1));
      Input::MouseButtonWentDown[SDL_BUTTON_LEFT] = false;
      return;
    }
  }
}

RectF LibraryMenu::CgCell(int entry) const {
  const int row = entry / CgColumns - CgTopRow;
  const int col = entry % CgColumns;
  return RectF(CgGridPos.x + col * CgCellPitch.x,
               CgGridPos.y + row * CgCellPitch.y, NoDataSprite.ScaledWidth(),
               NoDataSprite.ScaledHeight());
}

void LibraryMenu::UpdateCgInput(float dt) {
  const int count = CgCount();
  int next = CgCursor;
  if (PADinputButtonRepeatAccelDown & PAD1LEFT)
    next = std::max(CgCursor - 1, 0);
  if (PADinputButtonRepeatAccelDown & PAD1RIGHT)
    next = std::min(CgCursor + 1, count - 1);
  if (PADinputButtonRepeatAccelDown & PAD1UP && CgCursor >= CgColumns)
    next = CgCursor - CgColumns;
  if (PADinputButtonRepeatAccelDown & PAD1DOWN &&
      CgCursor / CgColumns < (count - 1) / CgColumns)
    next = std::min(CgCursor + CgColumns, count - 1);
  if (Input::MouseWheelDeltaY > 0 && CgCursor >= CgColumns)
    next = CgCursor - CgColumns;
  if (Input::MouseWheelDeltaY < 0 &&
      CgCursor / CgColumns < (count - 1) / CgColumns)
    next = std::min(CgCursor + CgColumns, count - 1);

  bool clicked = false;
  for (int i = CgTopRow * CgColumns;
       i < std::min(count, (CgTopRow + CgRows) * CgColumns); i++) {
    if (!CgCell(i).ContainsPoint(Input::CurMousePos)) continue;
    if (Input::PrevMousePos != Input::CurMousePos) next = i;
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
      next = i;
      clicked = true;
    }
  }

  if (next != CgCursor) {
    CgCursor = next;
    PlaySe(1);
  }
  const int row = CgCursor / CgColumns;
  if (row < CgTopRow) CgTopRow = row;
  if (row >= CgTopRow + CgRows) CgTopRow = row - CgRows + 1;

  if (PADinputButtonWentDown & PAD1A || clicked) {
    if (IsCgUnlocked(CgCursor)) {
      OpenViewer(CgCursor);
      PlaySe(2);
    }
  }
}

void LibraryMenu::OpenViewer(int entry) {
  ViewerVariations.clear();
  for (int v = 0; v < Profile::SaveSystem::MaxAlbumSubEntries; v++) {
    if (Profile::SaveSystem::AlbumEvData[entry][v] == 0xFFFF) break;
    if (IsCgVariationUnlocked(entry, v)) ViewerVariations.push_back(v);
  }
  if (ViewerVariations.empty()) return;
  ViewerEntry = entry;
  ViewerVariation = 0;
  ViewerOpen = true;
  ViewerGuide = true;
  RequestCg();
}

void LibraryMenu::RequestCg() {
  ScrWork[SW_ALBUM_LOADFILE] =
      Profile::SaveSystem::AlbumData[ViewerEntry]
                                    [ViewerVariations[ViewerVariation]][0];
  ScrWork[SW_ALBUM_LOADBUF] = CgLoadBuffer;
  SetFlag(SF_ALBUMLOAD_COMPLETE, false);
  SetFlag(SF_ALBUMLOAD, true);
  ViewerLoaded = false;
}

void LibraryMenu::CloseViewer() {
  SetFlag(SF_ALBUMLOAD, false);
  SetFlag(SF_ALBUMLOAD_COMPLETE, false);
  ViewerOpen = false;
  ViewerLoaded = false;
}

void LibraryMenu::ClampViewerPan() {
  const glm::vec2 screen(Profile::Game::DesignWidth,
                         Profile::Game::DesignHeight);
  const glm::vec2 size = ViewerSize * ViewerScale;
  for (int axis = 0; axis < 2; axis++) {
    ViewerPan[axis] =
        size[axis] <= screen[axis]
            ? (screen[axis] - size[axis]) / 2.0f
            : std::clamp(ViewerPan[axis], screen[axis] - size[axis], 0.0f);
  }
}

void LibraryMenu::UpdateViewerInput(float dt) {
  if (PADinputButtonWentDown & PAD1B ||
      Input::MouseButtonWentDown[SDL_BUTTON_RIGHT]) {
    CloseViewer();
    PlaySe(3);
    return;
  }
  if (PADinputButtonWentDown & PAD1X) ViewerGuide = !ViewerGuide;
  if (!ViewerLoaded) return;

  if (PADinputButtonWentDown & PAD1A ||
      Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
    if (ViewerVariation + 1 < ViewerVariations.size()) {
      ViewerVariation++;
      RequestCg();
      PlaySe(1);
    } else {
      CloseViewer();
      PlaySe(3);
    }
    return;
  }

  float zoom = 1.0f;
  if (PADinputButtonWentDown & PAD1L2) zoom = 1.0f / ZoomStep;
  if (PADinputButtonWentDown & PAD1R2) zoom = ZoomStep;
  if (zoom != 1.0f) {
    const float scale = std::clamp(ViewerScale * zoom, ViewerContainScale,
                                   ViewerCoverScale * CgZoomMax);
    const glm::vec2 center(Profile::Game::DesignWidth / 2.0f,
                           Profile::Game::DesignHeight / 2.0f);
    ViewerPan = center - (center - ViewerPan) * (scale / ViewerScale);
    ViewerScale = scale;
  }

  glm::vec2 move(0.0f);
  if (PADinputButtonIsDown & PAD1LEFT) move.x += 1.0f;
  if (PADinputButtonIsDown & PAD1RIGHT) move.x -= 1.0f;
  if (PADinputButtonIsDown & PAD1UP) move.y += 1.0f;
  if (PADinputButtonIsDown & PAD1DOWN) move.y -= 1.0f;
  ViewerPan += move * CgScrollSpeed * dt;
  ViewerPan.y += Input::MouseWheelDeltaY * CgScrollSpeed * 0.1f;
  ClampViewerPan();
}

void LibraryMenu::UpdateSoundInput(float dt) {
  const int count = static_cast<int>(TrackBgmIds.size());
  int next = TrackCursor;
  if (PADinputButtonRepeatAccelDown & PAD1UP)
    next = std::max(TrackCursor - 1, 0);
  if (PADinputButtonRepeatAccelDown & PAD1DOWN)
    next = std::min(TrackCursor + 1, count - 1);
  if (PADinputButtonRepeatAccelDown & PAD1LEFT)
    next = std::max(TrackCursor - ListVisibleRows, 0);
  if (PADinputButtonRepeatAccelDown & PAD1RIGHT)
    next = std::min(TrackCursor + ListVisibleRows, count - 1);
  if (Input::MouseWheelDeltaY != 0) {
    next = std::clamp(TrackCursor - static_cast<int>(Input::MouseWheelDeltaY),
                      0, count - 1);
  }

  bool clicked = false;
  const float headerHeight = ListHeaderSprite.ScaledHeight();
  for (int row = 0; row < ListVisibleRows && TrackTopRow + row < count; row++) {
    const RectF bounds(ListBounds.X,
                       ListBounds.Y + headerHeight + row * ListRowHeight,
                       ListBounds.Width, ListRowHeight);
    if (!bounds.ContainsPoint(Input::CurMousePos)) continue;
    if (Input::PrevMousePos != Input::CurMousePos) next = TrackTopRow + row;
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
      next = TrackTopRow + row;
      clicked = true;
    }
  }

  if (next != TrackCursor) {
    TrackCursor = next;
    PlaySe(1);
  }
  if (TrackCursor < TrackTopRow) TrackTopRow = TrackCursor;
  if (TrackCursor >= TrackTopRow + ListVisibleRows)
    TrackTopRow = TrackCursor - ListVisibleRows + 1;

  if ((PADinputButtonWentDown & PAD1A || clicked) &&
      IsTrackUnlocked(TrackCursor)) {
    PlayTrack(TrackCursor);
  }
  if (PADinputButtonWentDown & PAD1X) StopTrack();
  if (PADinputButtonWentDown & PAD1Y) {
    RepeatMode = (RepeatMode + 1) % 4;
    PlaySe(1);
  }
}

void LibraryMenu::PlayTrack(int track) {
  const int bgm = TrackBgmIds[track];
  Audio::Channels[Audio::AC_BGM0]->Play("bgm", bgm, RepeatMode == 0, 0.0f);
  // The library's closing BGMplay (the title theme) only plays when the
  // requested track differs, as for the scripts' own BGMplay.
  ScrWork[SW_BGMREQNO] = bgm;
  PlayingTrack = track;
}

void LibraryMenu::StopTrack() {
  Audio::Channels[Audio::AC_BGM0]->Stop(0.5f);
  ScrWork[SW_BGMREQNO] = 0xFFFF;
  PlayingTrack = -1;
}

void LibraryMenu::UpdatePlayback() {
  if (PlayingTrack < 0 || RepeatMode == 0) return;
  if (Audio::Channels[Audio::AC_BGM0]->GetState() != Audio::ACS_Stopped) return;

  const int count = static_cast<int>(TrackBgmIds.size());
  int next = -1;
  if (RepeatMode == 3) {
    std::vector<int> unlocked;
    for (int t = 0; t < count; t++) {
      if (t != PlayingTrack && IsTrackUnlocked(t)) unlocked.push_back(t);
    }
    next = unlocked.empty()
               ? PlayingTrack
               : unlocked[CALCrnd(static_cast<int>(unlocked.size()))];
  } else {
    for (int step = 1; step <= count; step++) {
      int t = PlayingTrack + step;
      if (t >= count) {
        if (RepeatMode == 1) break;
        t -= count;
      }
      if (IsTrackUnlocked(t)) {
        next = t;
        break;
      }
    }
  }
  if (next < 0) {
    PlayingTrack = -1;
    ScrWork[SW_BGMREQNO] = 0xFFFF;
    return;
  }
  PlayTrack(next);
}

RectF LibraryMenu::MovieCell(int movie) const {
  const Sprite& thumb = MovieThumbnailSprites[movie];
  const int row = movie / MovieColumns;
  const int col = movie % MovieColumns;
  return RectF(MovieGridPos.x + col * MovieCellPitch.x,
               MovieGridPos.y + row * MovieCellPitch.y,
               thumb.ScaledWidth() * MovieThumbnailScale,
               thumb.ScaledHeight() * MovieThumbnailScale);
}

void LibraryMenu::UpdateMovieInput(float dt) {
  const int count = static_cast<int>(MovieIds.size());
  int next = MovieCursor;
  if (PADinputButtonRepeatAccelDown & PAD1LEFT)
    next = std::max(MovieCursor - 1, 0);
  if (PADinputButtonRepeatAccelDown & PAD1RIGHT)
    next = std::min(MovieCursor + 1, count - 1);
  if (PADinputButtonRepeatAccelDown & PAD1UP && MovieCursor >= MovieColumns)
    next = MovieCursor - MovieColumns;
  if (PADinputButtonRepeatAccelDown & PAD1DOWN &&
      MovieCursor + MovieColumns < count)
    next = MovieCursor + MovieColumns;

  bool clicked = false;
  for (int i = 0; i < count; i++) {
    if (!MovieCell(i).ContainsPoint(Input::CurMousePos)) continue;
    if (Input::PrevMousePos != Input::CurMousePos) next = i;
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
      next = i;
      clicked = true;
    }
  }
  if (next != MovieCursor) {
    MovieCursor = next;
    PlaySe(1);
  }
  if ((PADinputButtonWentDown & PAD1A || clicked) &&
      IsMovieUnlocked(MovieCursor)) {
    ScrWork[SW_MOVIEMODE_CUR] = MovieIds[MovieCursor];
    PlaySe(2);
  }
}

void LibraryMenu::Render() {
  if (State == Hidden || ScrWork[SW_MOVIEMODE_CUR] != NoMovie) return;
  const float alpha = std::clamp(
      ScrWork[SW_SYSSUBMENUCT] / static_cast<float>(CountMax), 0.0f, 1.0f);
  const glm::vec4 tint(1.0f, 1.0f, 1.0f, alpha);
  if (ViewerOpen) {
    RenderViewer(tint);
    return;
  }
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f), tint);
  switch (Current) {
    case Library::Cg:
      RenderCg(tint);
      break;
    case Library::Sound:
      RenderSound(tint);
      break;
    case Library::Movie:
      RenderMovie(tint);
      break;
  }
  RenderTabs(tint);
  if (Current != Library::Sound) {
    Renderer->DrawSprite(TitleSprites[static_cast<int>(Current)], TitlePos,
                         tint);
  }
  RenderGuide(Current == Library::Cg      ? 0
              : Current == Library::Sound ? 2
                                          : 3,
              tint);
}

void LibraryMenu::RenderTabs(glm::vec4 tint) {
  const int selected = static_cast<int>(Current) + 1;
  for (int tab = 0; tab < TabCount; tab++) {
    const Sprite& sprite =
        tab == selected ? TabSelectedSprites[tab] : TabSprites[tab];
    Renderer->DrawSprite(sprite, TabPos + glm::vec2(0.0f, tab * TabPitch),
                         tint);
  }
}

void LibraryMenu::RenderGuide(int guide, glm::vec4 tint) {
  const Sprite& sprite = GuideSprites[guide];
  Renderer->DrawSprite(
      sprite, glm::vec2(GuideRight - sprite.ScaledWidth(), GuideY), tint);
}

void LibraryMenu::RenderCg(glm::vec4 tint) {
  const int count = CgCount();
  for (int i = CgTopRow * CgColumns;
       i < std::min(count, (CgTopRow + CgRows) * CgColumns); i++) {
    const RectF cell = CgCell(i);
    const glm::vec2 pos(cell.X, cell.Y);
    const bool selected = i == CgCursor;
    if (!IsCgUnlocked(i)) {
      Renderer->DrawSprite(selected ? NoDataSelectedSprite : NoDataSprite, pos,
                           tint);
      continue;
    }
    Renderer->DrawSprite(ThumbnailSprites[i], pos + ThumbnailOffset, tint);
    if (selected) {
      glm::vec4 glow = tint;
      glow.a *= 0.35f + 0.25f * std::sin(Time * 4.0f);
      Renderer->DrawSprite(SelectedGlowSprite, pos + ThumbnailOffset, glow);
      Renderer->DrawSprite(SelectedFrameSprite, pos, tint);
    }
  }

  const int rows = (count + CgColumns - 1) / CgColumns;
  const int maxTop = std::max(rows - CgRows, 1);
  const float travel =
      ScrollBottom - ScrollTop - ScrollThumbSprite.ScaledHeight();
  Renderer->DrawSprite(
      ScrollThumbSprite,
      glm::vec2(ScrollX,
                ScrollTop + travel * CgTopRow / static_cast<float>(maxTop)),
      tint);
}

void LibraryMenu::RenderViewer(glm::vec4 tint) {
  Renderer->DrawQuad(RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                           Profile::Game::DesignHeight),
                     glm::vec4(0.0f, 0.0f, 0.0f, tint.a));
  if (ViewerLoaded) {
    const glm::vec2 size = ViewerSize * ViewerScale;
    Renderer->DrawSprite(CgBackground()->BgSprite,
                         RectF(ViewerPan.x, ViewerPan.y, size.x, size.y), tint);
  }
  if (ViewerGuide) RenderGuide(1, tint);
}

void LibraryMenu::RenderSound(glm::vec4 tint) {
  Renderer->DrawSprite(PlayerSprite, PlayerPos, tint);

  const int count = static_cast<int>(TrackBgmIds.size());
  const float headerHeight = ListHeaderSprite.ScaledHeight();
  Renderer->EnableScissor();
  Renderer->SetScissorRect(ListBounds);
  Renderer->DrawSprite(ListHeaderSprite, glm::vec2(ListBounds.X, ListBounds.Y),
                       tint);
  for (int row = 0; row < ListVisibleRows && TrackTopRow + row < count; row++) {
    const int track = TrackTopRow + row;
    const float y = ListBounds.Y + headerHeight + row * ListRowHeight;
    const bool selected = track == TrackCursor;
    const bool unlocked = IsTrackUnlocked(track);
    Renderer->DrawSprite(
        selected ? ListSelectedSprite : ListRowSprites[track % 2],
        glm::vec2(ListBounds.X, y), tint);
    const uint32_t color = selected   ? SelectedTextColor
                           : unlocked ? TextColor
                                      : LockedTextColor;
    const float textY = y + (ListRowHeight - ListFontSize) / 2.0f;
    if (track == PlayingTrack) {
      const float icon = ListRowHeight - 20.0f;
      Renderer->DrawSprite(
          PlayIconSprite,
          RectF(ListBounds.X + ListTrackX, y + 10.0f, icon, icon), tint);
    } else {
      DrawText(LayoutAscii(fmt::format("{:02}", track + 1), ListFontSize),
               glm::vec2(ListBounds.X + ListTrackX, textY), tint.a, color);
    }
    DrawText(unlocked ? TrackTitles[track] : LockedTrackTitle,
             glm::vec2(ListBounds.X + ListTitleX, textY), tint.a, color);
    if (unlocked) {
      DrawText(TrackArtists[track],
               glm::vec2(ListBounds.X + ListArtistX, textY), tint.a, color);
    }
  }
  Renderer->DisableScissor();

  if (PlayingTrack >= 0) {
    const auto& channel = Audio::Channels[Audio::AC_BGM0];
    DrawText(TrackTitles[PlayingTrack], NowPlayingPos, tint.a, TextColor);
    const std::string time = FormatTime(channel->PositionInSeconds()) + " / " +
                             FormatTime(channel->DurationInSeconds());
    DrawText(LayoutAscii(time, NowPlayingFontSize),
             NowPlayingPos + glm::vec2(0.0f, NowPlayingFontSize + 8.0f), tint.a,
             TextColor);
  }
  Renderer->DrawSprite(RepeatModeSprites[RepeatMode], RepeatModePos, tint);
}

void LibraryMenu::RenderMovie(glm::vec4 tint) {
  for (int i = 0; i < static_cast<int>(MovieIds.size()); i++) {
    const RectF cell = MovieCell(i);
    const bool unlocked = IsMovieUnlocked(i);
    Renderer->DrawSprite(unlocked ? MovieThumbnailSprites[i] : NoDataSprite,
                         cell, tint);
    if (i == MovieCursor) {
      Renderer->DrawSprite(
          unlocked ? SelectedFrameSprite : NoDataSelectedSprite,
          RectF(cell.X - 4.0f, cell.Y - 4.0f, cell.Width + 8.0f,
                cell.Height + 8.0f),
          tint);
    }
    const uint32_t color = unlocked ? SelectedTextColor : LockedTextColor;
    const auto& title = unlocked ? MovieTitles[i] : LockedTrackTitle;
    const float centerX = cell.X + cell.Width / 2.0f;
    const float titleY = cell.Y + cell.Height + MovieTitleOffsetY;
    DrawText(title, glm::vec2(centerX - GetTextWidth(title) / 2.0f, titleY),
             tint.a, color);
    if (unlocked) {
      DrawText(MovieArtists[i],
               glm::vec2(centerX - GetTextWidth(MovieArtists[i]) / 2.0f,
                         titleY + MovieFontSize + 6.0f),
               tint.a, color);
    }
  }
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
