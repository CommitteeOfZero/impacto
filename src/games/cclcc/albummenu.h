#pragma once

#include "librarysubmenus.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

struct AlbumThumbnail : public Widgets::Button {
  enum class DisplayState : uint8_t {
    Hidden,
    Hiding,
    Showing,
    Shown,
  };

  AlbumThumbnail(int id, uint8_t const& activePage);
  void UpdateInput() override;
  void Render() override;
  void Show() override {
    if (State == DisplayState::Hidden && ActivePage == Page) {
      State = DisplayState::Shown;
    }
  }

  void Hide() override {
    if (State == DisplayState::Shown) {
      State = DisplayState::Hidden;
    }
  }

  DisplayState State = DisplayState::Hidden;
  uint8_t Page;
  uint8_t IndexInPage;
  uint8_t const& ActivePage;
};

class AlbumMenu : public LibrarySubmenu {
 public:
  AlbumMenu() = default;
  void Init() override;
  void Render() override;
  void Update(float dt) override;

  std::vector<std::vector<AlbumThumbnail*>> ThumbnailPages;
  uint8_t ActivePage = 0;
};
}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto