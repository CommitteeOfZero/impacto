#include <ranges>

#include "albummenu.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../data/savesystem.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/game.h"
#include "../../background2d.h"

using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;
namespace Impacto::UI::CCLCC {

void AlbumMenu::SetThumbnailDirections() {
  static constexpr int grid[] = {
      0x09, 0x0B, 0x63, 0x63, 0x04, 0x05, 0x06, 0x63, 0x03, 0x07, 0x63, 0x63,
      0x01, 0x05, 0x0A, 0x63, 0x0A, 0x09, 0x0B, 0x63, 0x05, 0x04, 0x06, 0x63,
      0x00, 0x04, 0x08, 0x09, 0x02, 0x06, 0x0B, 0x63, 0x0B, 0x09, 0x63, 0x63,
      0x06, 0x05, 0x04, 0x63, 0x01, 0x05, 0x0A, 0x63, 0x03, 0x07, 0x63, 0x63,
      0x0B, 0x09, 0x63, 0x63, 0x06, 0x05, 0x04, 0x63, 0x02, 0x06, 0x0B, 0x63,
      0x00, 0x04, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x08, 0x0A, 0x07, 0x63,
      0x03, 0x07, 0x63, 0x63, 0x05, 0x01, 0x0A, 0x63, 0x01, 0x00, 0x02, 0x03,
      0x0A, 0x08, 0x07, 0x63, 0x04, 0x08, 0x00, 0x09, 0x06, 0x02, 0x0B, 0x63,
      0x02, 0x03, 0x01, 0x00, 0x07, 0x0A, 0x08, 0x63, 0x05, 0x01, 0x0A, 0x63,
      0x03, 0x07, 0x63, 0x63, 0x06, 0x05, 0x04, 0x63, 0x0B, 0x09, 0x63, 0x63,
      0x0B, 0x06, 0x02, 0x63, 0x08, 0x09, 0x04, 0x00, 0x04, 0x05, 0x06, 0x63,
      0x09, 0x0B, 0x63, 0x63, 0x07, 0x03, 0x63, 0x63, 0x0A, 0x05, 0x01, 0x63,
      0x08, 0x0A, 0x07, 0x63, 0x00, 0x01, 0x02, 0x03, 0x07, 0x03, 0x63, 0x63,
      0x0A, 0x05, 0x01, 0x63, 0x05, 0x01, 0x63, 0x63, 0x01, 0x05, 0x63, 0x63,
      0x08, 0x09, 0x04, 0x00, 0x0B, 0x06, 0x02, 0x63, 0x07, 0x0A, 0x08, 0x63,
      0x02, 0x03, 0x01, 0x00, 0x0A, 0x05, 0x01, 0x63, 0x07, 0x03, 0x63, 0x63,
  };
  enum GridDirection {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
  };
  const auto convertDirection = [](FocusDirection dir) {
    switch (dir) {
      case FDIR_UP:
        return UP;
      case FDIR_DOWN:
        return DOWN;
      case FDIR_LEFT:
        return LEFT;
      case FDIR_RIGHT:
        return RIGHT;
    }
  };

  for (auto& page : ThumbnailPages) {
    for (auto& thumbnail : page) {
      if (!thumbnail) continue;
      auto nextGridIndex = thumbnail->GridId << 4;
      const auto setFocusDirection = [&](FocusDirection dir) {
        int i = 0;
        int compareId = thumbnail->GridId;
        for (int i = 0; i < 4; i++) {
          const auto* nextId = &grid[nextGridIndex + convertDirection(dir) * 4];
          uint8_t nextGridId = 0;
          for (int j = 0; j < 4; j++) {
            if (*nextId == 99) break;
            if (page.size() <= *nextId || !page[*nextId]) break;
            nextGridId = page[*nextId]->GridId;
            if (nextGridId == compareId) break;
            if (!page[nextGridId]->Variants.empty()) {
              thumbnail->SetFocus(page[nextGridId], dir);
              return;
            }
            nextId++;
          }
          compareId = grid[nextGridIndex + convertDirection(dir) * 4];
          nextGridIndex = compareId << 4;
        }
      };
      setFocusDirection(FDIR_UP);
      setFocusDirection(FDIR_DOWN);
      setFocusDirection(FDIR_LEFT);
      setFocusDirection(FDIR_RIGHT);
    }
  }
}

AlbumThumbnail::AlbumThumbnail(int id, uint8_t indexInPage, uint8_t gridId,
                               glm::vec2 gridDispPosition,
                               AlbumMenu const& albumMenu)
    : Widgets::Button(),
      GridId(gridId),
      GridPos(gridDispPosition),
      Menu(albumMenu) {
  Id = id;
  IndexInPage = AlbumData[id].IndexInPage;
  Page = AlbumData[id].PageNumber;
};

void AlbumThumbnail::UpdateInput() {
  if (Menu.ActivePage == Page) {
    Button::UpdateInput();
  }
}

void AlbumThumbnail::Update(float dt) {
  Button::Update(dt);
  if (State == DisplayState::Showing) {
    if (Menu.ThumbnailZoomAnimation.IsOut()) {
      State = DisplayState::Shown;
    }
  } else if (State == DisplayState::Hiding) {
    if (Menu.ThumbnailZoomAnimation.IsIn()) {
      State = DisplayState::Hidden;
    }
  }
}

void AlbumThumbnail::Show() {
  if (State == DisplayState::Hidden && Menu.ActivePage == Page) {
    State = DisplayState::Showing;
  }
}

void AlbumThumbnail::Hide() {
  if (State == DisplayState::Shown) {
    State = DisplayState::Hiding;
    HasFocus = false;
    Hovered = false;
  }
}

void AlbumThumbnail::Render() {
  if (!Enabled || State == DisplayState::Hidden) return;
  const float pgSwapDur = Menu.ThumbnailZoomAnimation.Progress;
  const glm::vec4 tint = Tint * glm::vec4(1.0f, 1.0f, 1.0f, 1 - pgSwapDur);
  ImpLog(LogLevel::Trace, LogChannel::General, "Page swap alpha: og:{}, adj:{}",
         Menu.ThumbnailZoomAnimation.Progress, pgSwapDur);
  for (const auto& spriteInfo : Variants) {
    const Sprite& thumbnailSprite = spriteInfo.ThumbnailSprite;
    const glm::vec2 picTopLeft =
        GridPos - glm::vec2(thumbnailSprite.Bounds.Width / 2, 0);
    const float scaleFactor = 1.5f * pgSwapDur + 1.0f;
    const auto matrix = TransformationMatrix(
        spriteInfo.Origin, {scaleFactor, scaleFactor}, spriteInfo.Origin,
        ScrWorkAngleToRad(spriteInfo.Angle), picTopLeft);
    Renderer->DrawSprite(thumbnailSprite, matrix, tint);
  }
  if (HasFocus && Menu.ThumbnailZoomAnimation.IsOut()) {
    const glm::vec2 thumbTopLeft =
        GridPos - glm::vec2(AlbumThumbnailThumbSprite.Bounds.Width / 2, 0);
    Renderer->DrawSprite(AlbumThumbnailThumbSprite, thumbTopLeft, Tint);
  }
  const auto& pinSprite = AlbumThumbnailPinSprites[IndexInPage];
  const glm::vec2 pinOffset = pgSwapDur * AlbumThumbnailPinRemoveOffset;
  const glm::vec2 pinTopLeft =
      GridPos - glm::vec2(pinSprite.Bounds.Width / 2, 0) - pinOffset;
  Renderer->DrawSprite(pinSprite, pinTopLeft, tint);
}

AlbumMenu::AlbumMenu() : LibrarySubmenu() {
  ThumbnailZoomAnimation.SetDuration(AlbumThumbZoomAnimationDuration);
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
  size_t index = 0;
  for (const auto& thumbnailEntry : AlbumData) {
    int itemCountInPage = 0;
    if (ThumbnailPages.size() <= thumbnailEntry.PageNumber) {
      ThumbnailPages.resize(thumbnailEntry.PageNumber + 1, {});
      itemCountInPage = 0;
    }
    auto& page = ThumbnailPages[thumbnailEntry.PageNumber];
    const glm::vec2 gridDispPosition =
        AlbumThumbDispPos[thumbnailEntry.IndexInPage];
    auto* thumbnailWidget =
        new AlbumThumbnail(i++, itemCountInPage, thumbnailEntry.IndexInPage,
                           gridDispPosition, *this);
    thumbnailWidget->OnClickHandler = [this, thumbnailWidget,
                                       index](Widgets::Button* btn) {
      auto* thumbnailPtr = static_cast<AlbumThumbnail*>(btn);
      ScrWork[SW_ALBUM_LOADFILE] = Profile::SaveSystem::AlbumData[index][0][0];
      ScrWork[SW_ALBUM_LOADBUF] = 1;
      SetFlag(SF_ALBUMLOAD, 1);
      SetFlag(SF_ALBUMLOAD_COMPLETE, 0);
      CGViewer = std::make_optional<AlbumCGViewer>(*thumbnailWidget);
      CGViewer->ActiveThumbnailIndex = index;
      CGViewer->ActiveVariantIndex = 0;
IsFocused = false;
      ThumbnailZoomAnimation.StartIn();
    };
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
          RectF{gridDispPosition.x - sprite.Bounds.Width / 2,
                gridDispPosition.y, sprite.Bounds.Width, sprite.Bounds.Height};
      if (maxBounds.Width == 0 || maxBounds.Height == 0) {
        maxBounds.X = variantBounds.Center().x;
        maxBounds.Y = variantBounds.Center().y;
      }
      variantBounds.Rotate(ScrWorkAngleToRad(variantAngle),
                           gridDispPosition + origin);
      maxBounds = RectF::Coalesce(variantBounds, maxBounds);
    }
    thumbnailWidget->Bounds = maxBounds;
    if (page.size() <= thumbnailEntry.IndexInPage) {
      page.resize(thumbnailEntry.IndexInPage + 1, nullptr);
    }
    page[thumbnailEntry.IndexInPage] = thumbnailWidget;
    MainItems.Add(thumbnailWidget);
    itemCountInPage++;
    index++;
  }
  for (const auto& thum : ThumbnailPages[ActivePage]) {
    if (!thum) continue;
    thum->Show();
    if (!CurrentlyFocusedElement) {
      CurrentlyFocusedElement = thum;
      CurrentlyFocusedElement->HasFocus = true;
    }
  }
  SetThumbnailDirections();
}

void AlbumMenu::UpdateCGViewer(float dt) {
  using namespace Vm::Interface;
  if (GetFlag(SF_ALBUMLOAD) && GetFlag(SF_ALBUMLOAD_COMPLETE)) {
    SetFlag(SF_ALBUMLOAD, 0);
    SetFlag(SF_ALBUMLOAD_COMPLETE, 0);
    int bufId = ScrWork[SW_BG1SURF];
    CGViewer->CGSprite = Backgrounds2D[bufId]->BgSprite;
    // Fit to height
    const auto heightRatio =
        Profile::DesignHeight / Backgrounds2D[bufId]->BgSprite.ScaledHeight();

    CGViewer->DestRect.Height =
        Backgrounds2D[bufId]->BgSprite.ScaledHeight() * heightRatio;
    CGViewer->DestRect.Width =
        Backgrounds2D[bufId]->BgSprite.ScaledWidth() * heightRatio;
    CGViewer->DestRect.X =
        (Profile::DesignWidth - CGViewer->DestRect.Width) / 2;
    CGViewer->DestRect.Y =
        (Profile::DesignHeight - CGViewer->DestRect.Height) / 2;
return;
  }
if (!ThumbnailZoomAnimation.IsIn()) return;
  if (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1A ||
      Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) {
    const auto& variants = CGViewer->ClickedThumbnail.get().Variants;
    CGViewer->ActiveVariantIndex++;
    if (CGViewer->ActiveVariantIndex < variants.size()) {
      ScrWork[SW_ALBUM_LOADFILE] =
          Profile::SaveSystem::AlbumData[CGViewer->ActiveThumbnailIndex]
                                        [CGViewer->ActiveVariantIndex][0];
      ScrWork[SW_ALBUM_LOADBUF] = 1;
      SetFlag(SF_ALBUMLOAD, 1);
      SetFlag(SF_ALBUMLOAD_COMPLETE, 0);
    } else {
      CGViewer = std::nullopt;
IsFocused = true;
      ThumbnailZoomAnimation.StartOut();
    }
  } else if (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1B ||
             Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1B) {
    CGViewer = std::nullopt;
IsFocused = true;
    ThumbnailZoomAnimation.StartOut();
  }
}

void AlbumMenu::UpdateThumbnail(float dt) {
  using namespace Vm::Interface;
    if (IsFocused) {
    const auto updatePages = [this](uint8_t prevPg, uint8_t nextPg) {
      if (prevPg != nextPg) {
        if (CurrentlyFocusedElement) {
          CurrentlyFocusedElement = nullptr;
        }
        const auto& prevPageThumbnails = ThumbnailPages[prevPg];
        for (const auto& thum : prevPageThumbnails) {
          if (!thum) continue;
          thum->Hide();
        }
        ThumbnailZoomAnimation.StartIn();
      }
    };
    auto prevPage = ActivePage;
if (ThumbnailZoomAnimation.IsOut()) {
    if (PADinputButtonWentDown & PADcustom[7] ||
Input::MouseWheelDeltaY > 0) {
      ActivePage =
          (ActivePage == 0) ? ThumbnailPages.size() - 1 : ActivePage - 1;
      updatePages(prevPage, ActivePage);
    } else if (PADinputButtonWentDown & PADcustom[8] ||
               Input::MouseWheelDeltaY < 0) {
      ActivePage = (ActivePage + 1) % ThumbnailPages.size();
      updatePages(prevPage, ActivePage);
    }
} else if (ThumbnailZoomAnimation.IsIn()) {
      const auto& curPage = ThumbnailPages[ActivePage];
      for (const auto& thum : curPage) {
        if (!thum) continue;
        thum->Show();
        if (!CurrentlyFocusedElement) {
          CurrentlyFocusedElement = thum;
          CurrentlyFocusedElement->HasFocus = true;
        }
      }
      ThumbnailZoomAnimation.StartOut();
    }
    if (!CurrentlyFocusedElement) {
      auto thumItr = std::find_if(ThumbnailPages[ActivePage].begin(),
                                  ThumbnailPages[ActivePage].end(),
                                  [this](const auto& btn) { return btn; });
      if (thumItr != ThumbnailPages[ActivePage].end()) {
        CurrentlyFocusedElement = *thumItr;
        CurrentlyFocusedElement->HasFocus = true;
      }
    }
      }
}

void AlbumMenu::Update(float dt) {
  MainItems.Tint.a = FadeAnimation.Progress;
const auto* prevBtn = CurrentlyFocusedElement;
  ThumbnailZoomAnimation.Update(dt);
  if (!CGViewer) {
LibrarySubmenu::Update(dt);
    UpdateThumbnail(dt);
if (CurrentlyFocusedElement != prevBtn) {
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
    }
  } else {
    UpdateCGViewer(dt);
  }
}

void AlbumMenu::Render() {
  LibrarySubmenu::Render();
  if (CGViewer) {
const glm::vec4 tint(1.0f, 1.0f, 1.0f, ThumbnailZoomAnimation.Progress);
    Renderer->DrawSprite(
        Sprite{}, RectF{0, 0, Profile::DesignWidth, Profile::DesignHeight},
        tint);
    Renderer->DrawSprite(CGViewer->CGSprite, CGViewer->DestRect, tint);
  }
}
}  // namespace Impacto::UI::CCLCC