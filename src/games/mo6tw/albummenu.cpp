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

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::AlbumMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

void AlbumMenu::CharacterButtonOnClick(Button* target) {
  if (!target->IsLocked) SwitchToCharacter(target->Id);
}

AlbumMenu::AlbumMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  MainItems = new Group(this);
  SecondaryItems = new Group(this);
  SecondaryItems->FocusLock = false;
  ImageGrid = new Group(this);
  ImageGrid->RenderingBounds = ThumbnailGridBounds;
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
      sprite = &CharacterButtonSprites[i];
      lockedSprite = &CharacterButtonSprites[i + 1];
      highlightedSprite = &HighlightedCharacterButtonSprites[i];
      highlightedLockedSprite = &HighlightedCharacterButtonSprites[i + 1];
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
}

void AlbumMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
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
    if (SelectedCharacterId != -1) ImageGrid->UpdateInput();
    if (PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) {
      if (SelectedCharacterId == -1) {
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
    if (SelectedCharacterId != -1) ImageGrid->Update(dt);
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
  }
}

void AlbumMenu::SwitchToCharacter(int id) {
  SelectedCharacterId = id;
  if (id == -1) {
    SecondaryItems->Hide();
    ImageGrid->Hide();
    MainItems->Show();
  } else {
    LoadCharacter(id);
    MainItems->Hide();
    SecondaryItems->Show();
    ImageGrid->Show();
  }
}

void AlbumMenu::LoadCharacter(int id) {
  ImageGrid->Clear();
  SecondaryItems->Clear();
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

  int row = 1;
  int endIdx =
      id == CharacterPortraitCount ? EventCgCount : ThumbnailOffsets[id + 1];
  int idx = 0;
  auto pos = ThumbnailGridFirstPosition;
  for (int i = ThumbnailOffsets[id]; i < endIdx; i++) {
    if (idx && idx % 3 == 0) {
      row += 1;
      pos.x = ThumbnailGridFirstPosition.x;
      pos.y += ThumbnailGridMargin.y;
    }
    int totalEvVariations, viewedEvVariations;
    SaveSystem::GetEVStatus(i, &totalEvVariations, &viewedEvVariations);
    auto button = new Widgets::MO6TW::AlbumThumbnailButton(
        i, Thumbnails[i], LockedThumbnail, ThumbnailHighlightTopLeft,
        ThumbnailHighlightTopRight, ThumbnailHighlightBottomLeft,
        ThumbnailHighlightBottomRight, ThumbnailBorder, pos);
    pos.x += ThumbnailGridMargin.x;
    button->IsLocked = viewedEvVariations == 0;
    ImageGrid->Add(button, FDIR_RIGHT);
    if (row != 1) {
      button->SetFocus(ImageGrid->Children.at(idx - 3), FDIR_UP);
    }
    idx += 1;
  }

  int totalRows = row;
  idx = 0;
  row = 1;
  for (const auto& el : ImageGrid->Children) {
    if (row != totalRows) {
      Widget* focusTarget;
      if ((idx + 3) > ImageGrid->Children.size() - 1)
        focusTarget = ImageGrid->Children.back();
      else
        focusTarget = ImageGrid->Children.at(idx + 3);
      el->SetFocus(focusTarget, FDIR_DOWN);
    }

    idx += 1;
    if (idx % 3 == 0) {
      row += 1;
    }
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto