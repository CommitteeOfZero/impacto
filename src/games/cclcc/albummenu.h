#pragma once

#include "librarysubmenus.h"

namespace Impacto {
namespace UI {
namespace CCLCC {
class AlbumMenu;

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

  AlbumThumbnail(int id, uint8_t gridId, glm::vec2 gridPos,
                 AlbumMenu const& albumMenu);
  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;
  void Show() override;
  void Hide() override;

  DisplayState State = DisplayState::Hidden;
  glm::vec2 GridPos;
  uint8_t Page;
  uint8_t IndexInPage;
  uint8_t GridId;
  AlbumMenu const& Menu;
  std::vector<AlbumThumbnailSpriteInfo> Variants;
};

struct AlbumCGViewer {
  bool EnableGuide = true;
  RectF DestRect[2];
  int ViewBufId[2]{0, 1};
  int ActiveThumbnailIndex{};
  int ActiveVariantIndex{};
  std::reference_wrapper<AlbumThumbnail> ClickedThumbnail;
  Animation PageSwapAnimation;
  bool WasClicked = false;
  float ClickHoldTime = 0.0f;
  float SnapWidthHoldTime = 0.0f;
  bool WasZoomHeld = false;
  AlbumCGViewer(AlbumThumbnail& thumbnail) : ClickedThumbnail(thumbnail) {}
  void CGViewerPanZoom(float dt);
};

class AlbumMenu : public LibrarySubmenu {
 public:
  AlbumMenu();
  void Init() override;
  void Update(float dt) override;
  void Render() override;

  std::vector<std::vector<AlbumThumbnail*>> ThumbnailPages;
  uint8_t ActivePage = 0;
  Animation ThumbnailZoomAnimation;

  std::optional<AlbumCGViewer> CGViewer;

 private:
  void SetThumbnailDirections();
  void UpdateThumbnail(float dt);
  void UpdateCGViewer(float dt);
};
}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto