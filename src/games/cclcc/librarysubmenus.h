#pragma once

#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/label.h"
#include <vector>
#include <memory>

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

  static auto GetSc3LockedText(glm::vec2 pos) {}

 private:
  std::array<ProcessedTextGlyph, 6> LockedText;
  std::array<ProcessedTextGlyph, 4> NumberText;
  Widgets::Label ArtistName;
  int Position = 0;
};
class LibrarySubmenu : public Menu {
 public:
  LibrarySubmenu() = default;

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;
  // void Move(glm::vec2 offset);
  // void MoveTo(glm::vec2 pos);

 protected:
  Animation FadeAnimation;
  Widgets::Group MainItems{this};
};

class AlbumMenu : public LibrarySubmenu {
 public:
  AlbumMenu() = default;
};

class MusicMenu : public LibrarySubmenu {
 public:
  enum class MusicPlayMode { Normal, Shuffle, Repeat, RepeatOne };
  MusicMenu();
  void Update(float dt) override;
  void Render() override;
  void Show() override;

 private:
  float PageY = 0;
  MusicPlayMode PlayMode = MusicPlayMode::Normal;
};

class MovieMenu : public LibrarySubmenu {
 public:
  MovieMenu();
  void Show() override;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto