#include "albummenu.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../data/savesystem.h"
#include <ranges>

using namespace Impacto::Profile::CCLCC::LibraryMenu;
namespace Impacto::UI::CCLCC {
AlbumThumbnail::AlbumThumbnail(int id, int indexInPage, glm::vec2 gridPos,
                               AlbumMenu const& albumMenu)
    : Widgets::Button(),
      IndexInPage(IndexInPage),
      GridPos(gridPos),
      Menu(albumMenu) {
  Id = id;
  IdInPage = AlbumData[id].IdInPage;
  Page = AlbumData[id].PageNumber;
};

void AlbumThumbnail::UpdateInput() {
  if (Menu.ActivePage == Page) {
    Button::UpdateInput();
  }
}

void AlbumThumbnail::Show() {
  if (State == DisplayState::Hidden && Menu.ActivePage == Page) {
    State = DisplayState::Shown;
  }
}

void AlbumThumbnail::Hide() {
  if (State == DisplayState::Shown) {
    State = DisplayState::Hidden;
  }
}

void AlbumThumbnail::Render() {
  if (!Enabled || State == DisplayState::Hidden) return;
  const glm::vec2 offset{
      (1.0f - Menu.FadeAnimation.Progress) * LibraryTransitionPositionOffset,
      0.0f};
  for (const auto& spriteInfo : Variants) {
    const Sprite& thumbnailSprite = spriteInfo.ThumbnailSprite;
    const glm::vec2 picTopLeft =
        GridPos - glm::vec2(thumbnailSprite.Bounds.Width / 2, 0) + offset;
    const auto matrix =
        TransformationMatrix(spriteInfo.Origin, {1.0f, 1.0f}, spriteInfo.Origin,
                             ScrWorkAngleToRad(spriteInfo.Angle), picTopLeft);
    Renderer->DrawSprite(thumbnailSprite, matrix, Tint);
  }
  if (HasFocus) {
    const glm::vec2 thumbTopLeft =
        GridPos - glm::vec2(AlbumThumbnailThumbSprite.Bounds.Width / 2, 0) +
        offset;
    Renderer->DrawSprite(AlbumThumbnailThumbSprite, thumbTopLeft);
  }
  const auto& pinSprite = AlbumThumbnailPinSprites[IdInPage];
  const glm::vec2 pinTopLeft =
      GridPos - glm::vec2(pinSprite.Bounds.Width / 2, 0) + offset;
  Renderer->DrawSprite(pinSprite, pinTopLeft);
}

void AlbumMenu::Init() {
  MainItems.Clear();
  ThumbnailPages.clear();
  int i = 0;
  const auto getMainAngle = [](int itemIndex) {
    switch (itemIndex) {
      case 0:
      case 6:
      case 7:
        return CALCrnd(910);

      case 1:
      case 2:
      case 4:
      case 8:
      case 9:
      case 10:
        return CALCrnd(1820) - 910;
      case 3:
      case 5:
        return 65536 - CALCrnd(910);
      default:  // yes, the 12th item will always have no angle
        return 0;
    }
  };
  for (const auto& thumbnailEntry : AlbumData) {
    if (ThumbnailPages.size() <= thumbnailEntry.PageNumber) {
      ThumbnailPages.resize(thumbnailEntry.PageNumber + 1, {});
    }

    const glm::vec2 gridPos = AlbumThumbDispPos[thumbnailEntry.IdInPage];
    const size_t itemCountInPage =
        ThumbnailPages[thumbnailEntry.PageNumber].size();
    auto* thumbnailWidget =
        new AlbumThumbnail(i++, itemCountInPage, gridPos, *this);
    const int mainAngle = getMainAngle(itemCountInPage);
    int variantAngleOffset = 0;
    RectF maxBounds;
    for (const auto& sprite :
         thumbnailEntry.ThumbnailSprites | std::views::reverse) {
      variantAngleOffset += 546 + CALCrnd(182);
      const glm::vec2 origin = {(sprite.Bounds.Width - 10.0) / 2.0f + 2.0f,
                                15.0f};
      const int variantAngle = mainAngle - variantAngleOffset / 2;
      thumbnailWidget->Variants.push_back(AlbumThumbnailSpriteInfo{
          .ThumbnailSprite = sprite, .Origin = origin, .Angle = variantAngle});
      CornersQuad variantBounds =
          RectF{gridPos.x - sprite.Bounds.Width / 2, gridPos.y,
                sprite.Bounds.Width, sprite.Bounds.Height};
      if (maxBounds.Width == 0 || maxBounds.Height == 0) {
        maxBounds.X = variantBounds.Center().x;
        maxBounds.Y = variantBounds.Center().y;
      }
      variantBounds.Rotate(ScrWorkAngleToRad(variantAngle), gridPos + origin);
      maxBounds = RectF::Coalesce(variantBounds, maxBounds);
    }
    thumbnailWidget->Bounds = maxBounds;
    ThumbnailPages[thumbnailEntry.PageNumber].push_back(thumbnailWidget);
    MainItems.Add(thumbnailWidget,
                  FocusDirection::FDIR_DOWN);  // fix directions
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

}  // namespace Impacto::UI::CCLCC