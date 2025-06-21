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
  void UpdateInput() override {};
  void Move(glm::vec2 relativePos) override;
  void MoveTo(glm::vec2 pos) override { Move(pos - Bounds.GetPos()); }
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
  int Position = 0;
};

class MusicMenu : public LibrarySubmenu {
 public:
  MusicMenu();
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;
  void Show() override;
  void Hide() override;

  float PageY = 0;
  MusicBGs BGWidget;
  Profile::CCLCC::LibraryMenu::MusicMenuPlayingMode PlayMode =
      Profile::CCLCC::LibraryMenu::MusicMenuPlayingMode::PlayAll;
  MusicTrackButton* CurrentlyPlayingBtn = nullptr;
  TurboOnHoldHandler DirectionButtonHoldHandler;
  Animation NowPlayingFadeAnimation;
  Widgets::Label NowPlayingTrackName;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto