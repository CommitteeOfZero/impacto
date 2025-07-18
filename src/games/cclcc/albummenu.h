#pragma once

#include "librarysubmenus.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

struct AlbumThumbnailSpriteInfo {
  std::reference_wrapper<const Sprite> ThumbnailSprite;
  glm::vec2 Origin;
  int Angle;
};

struct AlbumThumbnail : public Widgets::Button {
  enum class DisplayState : uint8_t {
    Hidden,
    Hiding,
    Showing,
    Shown,
  };

  AlbumThumbnail(int id, int indexInPage, glm::vec2 gridPos,
                 uint8_t const& activePage);
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
  glm::vec2 GridPos;
  uint8_t Page;
  uint8_t IdInPage;
  uint8_t IndexInPage;
  uint8_t const& ActivePage;
  std::vector<AlbumThumbnailSpriteInfo> Variants;
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