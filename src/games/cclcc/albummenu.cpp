#include "albummenu.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../data/savesystem.h"

using namespace Impacto::Profile::CCLCC::LibraryMenu;
namespace Impacto::UI::CCLCC {
AlbumThumbnail::AlbumThumbnail(int id, uint8_t const& activePage)
    : Widgets::Button(), ActivePage(activePage) {
  Id = id;
  IndexInPage = AlbumData[id].IndexInPage;
  Page = AlbumData[id].PageNumber;
  const glm::vec2 gridPos = AlbumThumbDispPos[IndexInPage];
  NormalSprite =
      AlbumData[id].ThumbnailSprites.at(0);  // use first sprite for now
  Bounds = RectF{gridPos.x, gridPos.y, NormalSprite.Bounds.Width,
                 NormalSprite.Bounds.Height};
};

void AlbumThumbnail::UpdateInput() {
  if (ActivePage == Page) {
    Button::UpdateInput();
  }
}

void AlbumThumbnail::Render() {
  if (!Enabled || State == DisplayState::Hidden) return;

  const glm::vec2 picTopLeft =
      glm::vec2(Bounds.X, Bounds.Y) - glm::vec2(Bounds.Width / 2, 0);
  Renderer->DrawSprite(NormalSprite, picTopLeft, Tint);
  if (HasFocus) {
    const glm::vec2 thumbTopLeft =
        glm::vec2(Bounds.X, Bounds.Y) -
        glm::vec2(AlbumThumbnailThumbSprite.Bounds.Width / 2, 0);
    Renderer->DrawSprite(AlbumThumbnailThumbSprite, thumbTopLeft);
  }
  const auto& pinSprite = AlbumThumbnailPinSprites[IndexInPage];
  const glm::vec2 pinTopLeft =
      glm::vec2(Bounds.X, Bounds.Y) - glm::vec2(pinSprite.Bounds.Width / 2, 0);
  Renderer->DrawSprite(pinSprite, pinTopLeft);
}

void AlbumMenu::Init() {
  if (MainItems.Children.empty()) {
    int i = 0;
    for (const auto& thumbnailEntry : AlbumData) {
      if (ThumbnailPages.size() <= thumbnailEntry.PageNumber) {
        ThumbnailPages.resize(thumbnailEntry.PageNumber + 1, {});
      }
      auto* thumbnailWidget = new AlbumThumbnail(i++, ActivePage);
      ThumbnailPages[thumbnailEntry.PageNumber].push_back(thumbnailWidget);
      MainItems.Add(thumbnailWidget,
                    FocusDirection::FDIR_DOWN);  // fix directions
    }
  }
}

void AlbumMenu::Update(float dt) {
  if (CurrentlyFocusedElement) {
    auto& thumbnail = static_cast<AlbumThumbnail&>(*CurrentlyFocusedElement);
    auto prevPage = ActivePage;
    ActivePage = thumbnail.Page;
    if (prevPage != ActivePage) {
      const auto& prevPageThumbnails = ThumbnailPages[prevPage];
      for (const auto& thum : prevPageThumbnails) {
        thum->Hide();
      }
      const auto& curPage = ThumbnailPages[thumbnail.Page];
      for (const auto& thum : curPage) {
        thum->Show();
      }
    }
  }
  LibrarySubmenu::Update(dt);
}

void AlbumMenu::Render() {
  if (State != Hidden) {
    MainItems.Render();
  }
}
}  // namespace Impacto::UI::CCLCC