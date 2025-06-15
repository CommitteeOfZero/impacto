#pragma once

#include "librarysubmenus.h"
#include "../../ui/turboonholdhandler.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class MusicTrackButton : public Widgets::Button {
 public:
  MusicTrackButton(int id, int position, glm::vec2 pos);

  void Show() override;
  void Update(float dt) override;
  void Render() override;
  void Move(glm::vec2 relativePosition) override;

  bool Selected = false;

 private:
  std::array<ProcessedTextGlyph, 6> LockedText;
  std::array<ProcessedTextGlyph, 4> NumberText;
  Widgets::Label ArtistName;
  int Position = 0;
};

class MusicMenu : public LibrarySubmenu {
 public:
  enum class MusicPlayMode { Normal, Shuffle, Repeat, RepeatOne };
  MusicMenu();
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;
  void Show() override;
  void Hide() override;

  float PageY = 0;
  MusicPlayMode PlayMode = MusicPlayMode::Normal;
  MusicTrackButton* CurrentlyPlayingBtn = nullptr;
  TurboOnHoldHandler DirectionButtonHoldHandler;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto