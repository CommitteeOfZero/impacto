#include "savemenu.h"

#include "../../profile/ui/savemenu.h"
#include "../../profile/games/cclcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/cclcc/saveentrybutton.h"
#include "../../data/savesystem.h"
#include "../../profile/game.h"
#include "../../games/cclcc/savesystem.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::SaveMenu;
using namespace Impacto::Profile::CCLCC::SaveMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::UI::Widgets::CCLCC;

Widget* EntryGrid[Pages][RowsPerPage][EntriesPerRow];

void SaveMenu::MenuButtonOnClick(Widgets::Button* target) {
  Impacto::SaveSystem::SaveType saveType =
      *ActiveMenuType == SaveMenuPageType::QuickLoad
          ? SaveSystem::SaveType::Quick
          : SaveSystem::SaveType::Full;
  int SaveStatus = SaveSystem::GetSaveStatus(saveType, target->Id);
  if (SaveStatus == 1 || *ActiveMenuType == SaveMenuPageType::Save) {
    ScrWork[SW_SAVEFILENO] = target->Id;
    ScrWork[SW_SAVEFILETYPE] = (int)saveType;
    ScrWork[SW_SAVEFILESTATUS] =
        SaveSystem::GetSaveStatus(saveType, ScrWork[SW_SAVEFILENO]);

    ChoiceMade = true;
    SetFlag(SF_SAVEPROTECTED,
            SaveSystem::GetSaveFlags(saveType, ScrWork[SW_SAVEFILENO]) &
                SaveSystem::SaveFlagsMode::WriteProtect);
  }
  if ((*ActiveMenuType == SaveMenuPageType::Load ||
       *ActiveMenuType == SaveMenuPageType::QuickLoad) &&
      SaveStatus == 0) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
    return;
  }
}

SaveMenu::SaveMenu() : UI::SaveMenu(), CommonMenu(FadeAnimation) {
  PageAnimation.LoopMode = AnimationLoopMode::Stop;
  PageAnimation.DurationIn = PageSwapDuration;
}

void SaveMenu::Show() {
  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  OpenedAsDirect = GetFlag(SF_SYSTEMMENUDIRECT);
  if (!ActiveMenuType.has_value()) {
    // remap 2 3 4 -> 0 1 2
    ActiveMenuType =
        magic_enum::enum_cast<SaveMenuPageType>(ScrWork[SW_SYSMENUCNO] - 2);
  }

  CommonMenu::OnShow(FadeInDuration, FadeOutDuration, FadeAnimation);

  if (State != Showing) {
    HasCleared = false;
    State = Showing;
    FadeAnimation.StartIn();
    int id = 0;
    Impacto::SaveSystem::SaveType saveType =
        *ActiveMenuType == SaveMenuPageType::QuickLoad
            ? SaveSystem::SaveType::Quick
            : SaveSystem::SaveType::Full;

    for (int p = 0; p < Pages; ++p) {
      MainItems[p] = new Widgets::Group(this);
      MainItems[p]->WrapFocus = false;
      MainItems[p]->FocusLock = false;

      for (int i = 0; i < RowsPerPage; i++) {
        // Start on right col
        for (int j = EntriesPerRow - 1; j >= 0; j--) {
          glm::vec2 buttonPos =
              (j == 0)
                  ? glm::vec2{EntryStartXL, EntryStartYL + (i * EntryYPadding)}
                  : glm::vec2{EntryStartXR, EntryStartYR + (i * EntryYPadding)};
          SaveEntryButton* saveEntryButton = new SaveEntryButton(
              id, EntryHighlightedBoxSprite[*ActiveMenuType],
              EntryHighlightedTextSprite[*ActiveMenuType], p, buttonPos,
              SlotLockedSprite[*ActiveMenuType], saveType,
              NoDataSprite[*ActiveMenuType], BrokenDataSprite[*ActiveMenuType]);

          saveEntryButton->OnClickHandler = onClick;
          id++;

          EntryGrid[p][i][j] = saveEntryButton;
          MainItems[p]->Add(saveEntryButton);

          saveEntryButton->RefreshInfo();
        }
      }
    }
    for (int p = 0; p < Pages; ++p) {
      for (int j = EntriesPerRow - 1; j >= 0; j--) {
        for (int i = 0; i < RowsPerPage; i++) {
          // Right to left, top to bottom, page wrap
          if (i == 0) {
            int nextPage = (p == 0) ? Pages - 1 : p - 1;
            EntryGrid[p][i][j]->SetFocus(
                EntryGrid[nextPage][RowsPerPage - 1][j], FDIR_UP);
            EntryGrid[nextPage][RowsPerPage - 1][j]->SetFocus(
                EntryGrid[p][i][j], FDIR_DOWN);
            EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i + 1][j], FDIR_DOWN);
            EntryGrid[p][i + 1][j]->SetFocus(EntryGrid[p][i][j], FDIR_UP);
            if (j == 1) {
              EntryGrid[p][i][j]->SetFocus(
                  EntryGrid[nextPage][RowsPerPage - 1][0], FDIR_RIGHT);
              EntryGrid[nextPage][RowsPerPage - 1][0]->SetFocus(
                  EntryGrid[p][i][j], FDIR_LEFT);
              EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i][j - 1], FDIR_LEFT);
              EntryGrid[p][i][j - 1]->SetFocus(EntryGrid[p][i][j], FDIR_RIGHT);
            } else {
              EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i][j + 1], FDIR_RIGHT);
              EntryGrid[p][i][j + 1]->SetFocus(EntryGrid[p][i][j], FDIR_LEFT);
              EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i + 1][j + 1],
                                           FDIR_LEFT);
              EntryGrid[p][i + 1][j + 1]->SetFocus(EntryGrid[p][i][j],
                                                   FDIR_RIGHT);
            }
          } else if (i == RowsPerPage - 1) {
            int nextPage = (p + 1) % Pages;
            EntryGrid[p][i][j]->SetFocus(EntryGrid[nextPage][0][j], FDIR_DOWN);
            EntryGrid[nextPage][0][j]->SetFocus(EntryGrid[p][i][j], FDIR_UP);
            EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i - 1][j], FDIR_UP);
            EntryGrid[p][i - 1][j]->SetFocus(EntryGrid[p][i][j], FDIR_DOWN);
            if (j == 0) {
              EntryGrid[p][i][j]->SetFocus(EntryGrid[nextPage][0][1],
                                           FDIR_LEFT);
              EntryGrid[nextPage][0][1]->SetFocus(EntryGrid[p][i][j],
                                                  FDIR_RIGHT);
              EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i][j + 1], FDIR_RIGHT);
              EntryGrid[p][i][j + 1]->SetFocus(EntryGrid[p][i][j], FDIR_LEFT);
            } else {
              EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i][j - 1], FDIR_LEFT);
              EntryGrid[p][i][j - 1]->SetFocus(EntryGrid[p][i][j], FDIR_RIGHT);
              EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i - 1][j - 1],
                                           FDIR_RIGHT);
              EntryGrid[p][i - 1][j - 1]->SetFocus(EntryGrid[p][i][j],
                                                   FDIR_LEFT);
            }
          } else {
            int nextCol = (j + 1) % EntriesPerRow;
            int nextRow = j == 0 ? i + 1 : i;
            int prevRow = j == 1 ? i - 1 : i;
            EntryGrid[p][i][j]->SetFocus(EntryGrid[p][nextRow][nextCol],
                                         FDIR_LEFT);
            EntryGrid[p][nextRow][nextCol]->SetFocus(EntryGrid[p][i][j],
                                                     FDIR_RIGHT);
            EntryGrid[p][i][j]->SetFocus(EntryGrid[p][prevRow][nextCol],
                                         FDIR_RIGHT);
            EntryGrid[p][prevRow][nextCol]->SetFocus(EntryGrid[p][i][j],
                                                     FDIR_LEFT);
            EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i + 1][j], FDIR_DOWN);
            EntryGrid[p][i + 1][j]->SetFocus(EntryGrid[p][i][j], FDIR_UP);
            EntryGrid[p][i][j]->SetFocus(EntryGrid[p][i - 1][j], FDIR_UP);
            EntryGrid[p][i - 1][j]->SetFocus(EntryGrid[p][i][j], FDIR_DOWN);
          }
        }
      }
    }
    MainItems[CurrentPage]->Show();
    CurrentlyFocusedElement = MainItems[CurrentPage]->Children[0];
    CurrentlyFocusedElement->HasFocus = true;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;
  }
}
void SaveMenu::Hide() {
  if (State != Hiding) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SaveMenu::UpdateInput(float dt) {
  using namespace Vm::Interface;
  Menu::UpdateInput(dt);
  const auto updatePage = [&](int nextPage) {
    PrevPage = CurrentPage;
    if (CurrentlyFocusedElement) {
      LastFocusedEntry =
          static_cast<Widgets::Button*>(CurrentlyFocusedElement)->Id %
          (RowsPerPage * EntriesPerRow);
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement->Hovered = false;
    }
    CurrentPage = nextPage;
    MainItems[CurrentPage]->Show();
    if (LastFocusedEntry) {
      CurrentlyFocusedElement =
          MainItems[CurrentPage]->Children[*LastFocusedEntry];
    } else {
      CurrentlyFocusedElement = MainItems[CurrentPage]->Children.front();
    }
    PageAnimation.StartIn();
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  };
  if (IsFocused) {
    MainItems[CurrentPage]->UpdateInput(dt);
    if (Input::MouseWheelDeltaY < 0 || PADinputButtonWentDown & PADcustom[8]) {
      updatePage((CurrentPage + 1) % Pages);
      IsFocused = false;
    } else if (Input::MouseWheelDeltaY > 0 ||
               PADinputButtonWentDown & PADcustom[7]) {
      updatePage((CurrentPage - 1 + Pages) % Pages);
      IsFocused = false;
    } else {
      if (PADinputButtonRepeatAccelDown &
          (PAD1DOWN | PAD1UP | PAD1RIGHT | PAD1LEFT)) {
        Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
      }
    }

    if (CurrentlyFocusedElement && (PADinputButtonWentDown & PAD1Y)) {
      Impacto::SaveSystem::SaveType saveType =
          *ActiveMenuType == SaveMenuPageType::QuickLoad
              ? SaveSystem::SaveType::Quick
              : SaveSystem::SaveType::Full;
      auto saveButton = static_cast<SaveEntryButton*>(CurrentlyFocusedElement);
      if (SaveSystem::GetSaveStatus(saveType, saveButton->Id) == 1) {
        saveButton->ToggleLock();
        saveButton->RefreshInfo();
        Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
      } else {
        Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
      }
    }
  }
}

void SaveMenu::Update(float dt) {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 0 || ScrWork[SW_SYSSUBMENUNO] == 3 ||
              ScrWork[SW_SYSSUBMENUNO] == 4)) {
    Show();
  }

  if (State != Hidden) {
    FadeAnimation.Update(dt);
    PageAnimation.Update(dt);
  }

  if (State == Shown &&
      (ScrWork[SW_SYSSUBMENUNO] == 0 || ScrWork[SW_SYSSUBMENUNO] == 3 ||
       ScrWork[SW_SYSSUBMENUNO] == 4)) {
    UpdateInput(dt);
  }

  if (State == Showing && FadeAnimation.Progress == 1.0f &&
      ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    ActiveMenuType = std::nullopt;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }

  if (State == Shown && IsFocused && CurrentlyFocusedElement) {
    int oldPage = CurrentPage;
    MainItems[CurrentPage]->Update(dt);
    CurrentPage =
        dynamic_cast<SaveEntryButton*>(CurrentlyFocusedElement)->GetPage();
    if (CurrentPage != oldPage) {
      auto focusedElem = CurrentlyFocusedElement;
      PageAnimation.StartIn();
      MainItems[CurrentPage]->Show();
      CurrentlyFocusedElement = focusedElem;
      CurrentlyFocusedElement->HasFocus = true;
      IsFocused = false;
    }
  }
  if (State == Hidden && !HasCleared) {
    for (int p = 0; p < Pages; ++p) {
      MainItems[p]->Clear();
      delete MainItems[p];
      MainItems[p] = nullptr;
    }
    CurrentlyFocusedElement = nullptr;
    HasCleared = true;
  }
  if (PageAnimation.IsIn()) {
    MainItems[PrevPage]->MoveTo({0, 0});
    MainItems[PrevPage]->Hide();
    PageAnimation.Progress = 0.0f;
    PrevPage = CurrentPage;
    IsFocused = true;
    if (CurrentlyFocusedElement) CurrentlyFocusedElement->HasFocus = true;
  }
}

void SaveMenu::Render() {
  if (State == Hidden) return;

  const glm::vec4 col(1.0f, 1.0f, 1.0f,
                      glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
  const float transitionProgress =
      OpenedAsDirect ? 1.0f : FadeAnimation.Progress;
  const glm::vec2 transitionOffset = {
      transitionProgress * 32 * 200 * 0.0625 - 400, 0};

  if (OpenedAsDirect) CommonMenu::DrawBgSprite<false>(State, FadeAnimation);

  Renderer->DrawSprite(MenuTextSprite[*ActiveMenuType],
                       MenuTextPosition + transitionOffset, col);
  MainItems[CurrentPage]->Tint = col;
  if (PageAnimation.State == AnimationState::Playing) {
    bool isNextBelow = ((PrevPage + 1) % Pages) == CurrentPage;
    const float currentYPos =
        (1.0f - PageAnimation.Progress) *
        (isNextBelow ? Profile::DesignHeight : -Profile::DesignHeight);
    const float prevYPos = isNextBelow ? currentYPos - Profile::DesignHeight
                                       : currentYPos + Profile::DesignHeight;
    const glm::vec2 currentOffset{0, currentYPos};
    const glm::vec2 prevOffset{0, prevYPos};

    Renderer->DrawSprite(
        EntrySlotsSprite[*ActiveMenuType],
        SlotsBackgroundPosition + transitionOffset + prevOffset, col);
    Renderer->DrawSprite(
        EntrySlotsSprite[*ActiveMenuType],
        SlotsBackgroundPosition + transitionOffset + currentOffset, col);
    MainItems[PrevPage]->MoveTo(transitionOffset + prevOffset);
    MainItems[CurrentPage]->MoveTo(transitionOffset + currentOffset);
    MainItems[PrevPage]->Render();
    MainItems[CurrentPage]->Render();

    const glm::vec2 prevPgNumPos =
        PageNumberPosition + transitionOffset + prevOffset;
    const glm::vec2 curPgNumPos =
        PageNumberPosition + transitionOffset + currentOffset;
    Renderer->DrawSprite(PageNumSprite[*ActiveMenuType][PrevPage], prevPgNumPos,
                         col);
    Renderer->DrawSprite(PageNumSprite[*ActiveMenuType][CurrentPage],
                         curPgNumPos, col);
  } else {
    Renderer->DrawSprite(EntrySlotsSprite[*ActiveMenuType],
                         SlotsBackgroundPosition + transitionOffset, col);
    MainItems[CurrentPage]->MoveTo(transitionOffset);
    MainItems[CurrentPage]->Render();
    Renderer->DrawSprite(PageNumSprite[*ActiveMenuType][CurrentPage],
                         PageNumberPosition + transitionOffset, col);
  }

  if (ScrWork[SW_SYSSUBMENUNO] == 0 || ScrWork[SW_SYSSUBMENUNO] == 3 ||
      ScrWork[SW_SYSSUBMENUNO] == 4) {
    CommonMenu::DrawOverlay();
    if (GetFlag(SF_TITLEMODE)) {
      CommonMenu::DrawSmoke(Profile::CCLCC::SystemMenu::SmokeOpacityNormal *
                            (1.0f - FadeAnimation.Progress));
    }
  }

  Renderer->DrawSprite(ButtonGuideSprite[*ActiveMenuType], {0, 989}, col);
}

void SaveMenu::RefreshCurrentEntryInfo() {
  if (!CurrentlyFocusedElement) return;
  static_cast<SaveEntryButton*>(CurrentlyFocusedElement)->RefreshInfo();
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto