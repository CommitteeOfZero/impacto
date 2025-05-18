#pragma once

#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"
#include <vector>
#include <memory>

namespace Impacto {
namespace UI {
namespace CCLCC {

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
  MusicMenu() = default;
};

class MovieMenu : public LibrarySubmenu {
 public:
  MovieMenu();
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto