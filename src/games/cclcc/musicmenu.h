#pragma once

#include <optional>
#include "librarysubmenus.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../ui/turboonholdhandler.h"
#include "../../ui/widget.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

struct MusicBGs : public UI::Widget {
  using UI::Widget::Move;
  using UI::Widget::MoveTo;
  void UpdateInput(float dt) override {};
  void Move(glm::vec2 relativePos) override;
  void Render() override;
};

class MusicTrackButton : public Widgets::Button {
 public:
  MusicTrackButton(int id, int position, glm::vec2 pos);

  void Show() override;
  void Update(float dt) override;
  void Render() override;
  void Move(glm::vec2 relativePosition) override;

  bool Selected = false;
  bool PrevFocusState = false;

 private:
  std::array<ProcessedTextGlyph, 6> LockedText;
  std::array<ProcessedTextGlyph, 4> NumberText;
  Widgets::Label ArtistName;
};

class MusicModeButton : public Widgets::Button {
 public:
  MusicModeButton(Profile::CCLCC::LibraryMenu::MusicMenuPlayingMode& mode);

  void Update(float dt) override;

 private:
  Profile::CCLCC::LibraryMenu::MusicMenuPlayingMode& PlayMode;
};

class MusicMenu : public LibrarySubmenu {
 public:
  MusicMenu();
  void Init() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;
  void Show() override;
  void Hide() override;
  void Unfocus() override;

 private:
  Profile::CCLCC::LibraryMenu::MusicMenuPlayingMode PlayMode =
      Profile::CCLCC::LibraryMenu::MusicMenuPlayingMode::RepeatAll;
  MusicTrackButton* CurrentlyPlayingBtn = nullptr;
  std::optional<FocusDirection> QueuedMove;
  std::vector<size_t> ShuffleTrackIndices;
  MusicBGs BGWidget;
  TurboOnHoldHandler DirectionButtonHoldHandler;
  Animation NowPlayingFadeAnimation;
  Widgets::Label NowPlayingTrackName;
  MusicModeButton ModeButton;
  bool TurboMoved = false;
  void PlayTrack(size_t index);
  void StopMusic();
  void ResetShuffle();
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto