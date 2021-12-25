#include "albummenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/albummenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../data/savesystem.h"
#include "../../background2d.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/mo6tw/albumcharacterbutton.h"
#include "../../ui/widgets/mo6tw/albumthumbnailbutton.h"
#include "../../profile/data/savesystem.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::AlbumMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

void AlbumMenu::CharacterButtonOnClick(Button* target) {
  if (!(target->Id == YunoButtonIdx && target->IsLocked))
    SwitchToCharacter(target->Id);
}

void AlbumMenu::ArrowUpOnClick(Widgets::Button* target) { MoveImageGrid(); }

void AlbumMenu::ArrowDownOnClick(Widgets::Button* target) { MoveImageGrid(); }

void AlbumMenu::CgOnClick(Widgets::Button* target) {
  ShowCgViewer = true;
  CgViewerWidget->LoadCgSprites(target->Id, "bg",
                                Profile::SaveSystem::AlbumData[target->Id]);
}

void AlbumMenu::OnCgVariationEnd(Widgets::CgViewer* target) {
  CgViewerGroup->Hide();
  ShowCgViewer = false;
}

AlbumMenu::AlbumMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  ArrowsAnimation.Direction = 1;
  ArrowsAnimation.LoopMode = ALM_ReverseDirection;
  ArrowsAnimation.DurationIn = ArrowsAnimationDuration;
  ArrowsAnimation.DurationOut = ArrowsAnimationDuration;
  ArrowsAnimation.StartIn();

  CgViewerWidget = new CgViewer();
  CgViewerWidget->OnVariationEndHandler =
      std::bind(&AlbumMenu::OnCgVariationEnd, this, std::placeholders::_1);
  CgViewerGroup = new Group(this);
  CgViewerGroup->Add(CgViewerWidget, FDIR_DOWN);

  MainItems = new Group(this);
  SecondaryItems = new Group(this);
  SecondaryItems->FocusLock = false;
  ImageGrid = new Group(this);
  ImageGrid->RenderingBounds = ThumbnailGridBounds;
  ImageGrid->WrapFocus = false;
  auto pos = InitialButtonPosition;
  int idx = 0;

  auto characterOnClick = std::bind(&AlbumMenu::CharacterButtonOnClick, this,
                                    std::placeholders::_1);

  for (int i = 0; i < CharacterButtonCount; i++) {
    if (idx % 2 == 0)
      pos.x = ButtonEvenX;
    else
      pos.x = ButtonOddX;

    Sprite *sprite, *lockedSprite, *highlightedSprite, *highlightedLockedSprite;
    if (idx == YunoButtonIdx || idx == SuzuButtonIdx) {
      sprite = &CharacterButtonSprites[i + 1];
      lockedSprite = &CharacterButtonSprites[i];
      highlightedSprite = &HighlightedCharacterButtonSprites[i + 1];
      highlightedLockedSprite = &HighlightedCharacterButtonSprites[i];
      i += 1;
    } else {
      sprite = &CharacterButtonSprites[i];
      lockedSprite = sprite;
      highlightedSprite = &HighlightedCharacterButtonSprites[i];
      highlightedLockedSprite = highlightedSprite;
    }
    auto button = new Widgets::MO6TW::AlbumCharacterButton(
        idx, *sprite, *lockedSprite, *highlightedSprite,
        *highlightedLockedSprite, pos, HighlightAnimationDuration);
    button->OnClickHandler = characterOnClick;
    MainItems->Add(button, FDIR_DOWN);
    pos += ButtonMargin;
    idx += 1;
  }

  Arrows = new Group(this);
  Arrows->FocusLock = false;
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  ArrowUpButton = new Button(0, ArrowUp, ArrowUp, nullSprite, ArrowUpPosition);
  ArrowUpButton->DisabledSprite = nullSprite;
  ArrowUpButton->OnClickHandler =
      std::bind(&AlbumMenu::ArrowUpOnClick, this, std::placeholders::_1);
  ArrowDownButton =
      new Button(1, ArrowDown, ArrowDown, nullSprite, ArrowDownPosition);
  ArrowDownButton->DisabledSprite = nullSprite;
  ArrowDownButton->OnClickHandler =
      std::bind(&AlbumMenu::ArrowDownOnClick, this, std::placeholders::_1);
  Arrows->Add(ArrowUpButton);
  Arrows->Add(ArrowDownButton);
}

void AlbumMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();

    auto suzuButton =
        static_cast<Button*>(MainItems->Children.at(SuzuButtonIdx));
    suzuButton->IsLocked = !GetFlag(830);

    int endIdx = YunoButtonIdx == CharacterPortraitCount
                     ? EventCgCount
                     : ThumbnailOffsets[YunoButtonIdx + 1];
    int totalEvVariations = 0, viewedEvVariations = 0;
    for (int i = ThumbnailOffsets[YunoButtonIdx]; i < endIdx; i++) {
      int total, viewed;
      SaveSystem::GetEVStatus(i, &total, &viewed);
      totalEvVariations += total;
      viewedEvVariations += viewed;
    }
    auto yunoButton =
        static_cast<Button*>(MainItems->Children.at(YunoButtonIdx));
    yunoButton->IsLocked = viewedEvVariations == 0;

    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void AlbumMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void AlbumMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    if (SelectedCharacterId != -1 && !ShowCgViewer) {
      if ((PADinputButtonWentDown & PAD1DOWN ||
           PADinputButtonWentDown & PAD1UP) &&
          ImageGrid->HasFocus) {
        MoveImageGrid();
      }
    }
    if (PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) {
      if (CgViewerGroup->IsShown) {
        CgViewerGroup->Hide();
        ShowCgViewer = false;
      } else if (SelectedCharacterId == -1) {
        SetFlag(SF_ALBUMEND, true);
      } else {
        SwitchToCharacter(-1);
      }
    }
  }
}

void AlbumMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_ALBUM_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_ALBUM_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_ALBUM_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    MainItems->Update(dt);
    if (SelectedCharacterId != -1) {
      if (ShowCgViewer) {
        if (!CgViewerGroup->IsShown) CgViewerGroup->Show();
        CgViewerGroup->Update(dt);
      } else {
        ArrowsAnimation.Update(dt);
        ImageGrid->Update(dt);
        if (ImageGrid->Bounds.Y == MaximumImageGridY) {
          ArrowUpButton->Enabled = false;
        } else if (ImageGrid->Bounds.Y == MinimumImageGridY) {
          ArrowDownButton->Enabled = false;
        } else {
          ArrowUpButton->Enabled = true;
          ArrowDownButton->Enabled = true;
        }
      }
    }
  }
}

void AlbumMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    MainItems->Tint = col;
    MainItems->Render();
    SecondaryItems->Tint = col;
    SecondaryItems->Render();
    ImageGrid->Tint = col;
    ImageGrid->Render();
    Arrows->Tint =
        glm::vec4(1.0f, 1.0f, 1.0f, glm::step(0.5f, ArrowsAnimation.Progress));
    Arrows->Render();
    CgViewerGroup->Render();
  }
}

void AlbumMenu::SwitchToCharacter(int id) {
  SelectedCharacterId = id;
  if (id == -1) {
    SecondaryItems->Hide();
    ImageGrid->Hide();
    Arrows->Hide();
    MainItems->Show();
  } else {
    LoadCharacter(id);
    MainItems->Hide();
    SecondaryItems->Show();
    Arrows->Show();
    ImageGrid->Show();
    FocusStart[FDIR_DOWN] = ImageGrid;
  }
}

void AlbumMenu::LoadCharacter(int id) {
  ImageGrid->Clear();
  SecondaryItems->Clear();
  ImageGrid->Bounds = ThumbnailGridBounds;
  // Others category portrait is split into two sprites
  if (id < CharacterPortraitCount) {
    SecondaryItems->Add(new Label(
        CharacterPortraits[id],
        glm::vec2(PortraitPosition.x - CharacterPortraits[id].ScaledWidth(),
                  PortraitPosition.y)));
  } else {
    SecondaryItems->Add(new Label(
        OthersPortraitTopPart,
        glm::vec2(
            OthersPortraitPosition.x - OthersPortraitTopPart.ScaledWidth(),
            OthersPortraitPosition.y)));
    SecondaryItems->Add(new Label(
        OthersPortraitBottomPart,
        glm::vec2(
            OthersPortraitPosition.x - OthersPortraitBottomPart.ScaledWidth(),
            OthersPortraitPosition.y + OthersPortraitTopPart.ScaledHeight())));
  }

  auto cgOnClick =
      std::bind(&AlbumMenu::CgOnClick, this, std::placeholders::_1);

  int row = 1;
  int endIdx =
      id == CharacterPortraitCount ? EventCgCount : ThumbnailOffsets[id + 1];
  int idx = 0;
  auto pos = ThumbnailGridFirstPosition;
  for (int i = ThumbnailOffsets[id]; i < endIdx; i++) {
    if (idx && idx % ThumbnailsPerRow == 0) {
      row += 1;
      pos.x = ThumbnailGridFirstPosition.x;
      pos.y += ThumbnailGridMargin.y;
    }
    int totalEvVariations, viewedEvVariations;
    SaveSystem::GetEVStatus(i, &totalEvVariations, &viewedEvVariations);
    auto button = new Widgets::MO6TW::AlbumThumbnailButton(
        i, Thumbnails[i], LockedThumbnail, ThumbnailHighlightTopLeft,
        ThumbnailHighlightTopRight, ThumbnailHighlightBottomLeft,
        ThumbnailHighlightBottomRight, ThumbnailBorder, viewedEvVariations,
        totalEvVariations, pos);
    pos.x += ThumbnailGridMargin.x;
    button->IsLocked = viewedEvVariations == 0;
    button->OnClickHandler = cgOnClick;
    ImageGrid->Add(button, FDIR_RIGHT);
    if (row != 1) {
      button->SetFocus(ImageGrid->Children.at(idx - ThumbnailsPerRow), FDIR_UP);
    }
    idx += 1;
  }

  int totalRows = row;
  idx = 0;
  row = 1;
  for (const auto& el : ImageGrid->Children) {
    if (row != totalRows) {
      Widget* focusTarget;
      if ((idx + ThumbnailsPerRow) > ImageGrid->Children.size() - 1)
        focusTarget = ImageGrid->Children.back();
      else
        focusTarget = ImageGrid->Children.at(idx + ThumbnailsPerRow);
      el->SetFocus(focusTarget, FDIR_DOWN);
    }

    idx += 1;
    if (idx % ThumbnailsPerRow == 0) {
      row += 1;
    }
  }

  MaximumImageGridY = ImageGrid->Bounds.Y;
  MinimumImageGridY =
      ImageGrid->Bounds.Y - (row - ThumbnailsPerColumn) * ThumbnailGridMargin.y;
}

void AlbumMenu::MoveImageGrid() {
  auto focusedEl = CurrentlyFocusedElement;
  if (focusedEl) {
    if (focusedEl->Bounds.Y < ImageGrid->RenderingBounds.Y) {
      ImageGrid->Move(glm::vec2(0.0f, ThumbnailGridMargin.y));
    } else if (focusedEl->Bounds.Y + focusedEl->Bounds.Height >
               ImageGrid->RenderingBounds.Y +
                   ImageGrid->RenderingBounds.Height) {
      ImageGrid->Move(glm::vec2(0.0f, -ThumbnailGridMargin.y));
    }
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto