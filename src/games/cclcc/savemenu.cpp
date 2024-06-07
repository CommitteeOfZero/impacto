#include "savemenu.h"

#include "../../profile/ui/savemenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/cclcc/saveentrybutton.h"
#include "../../data/savesystem.h"
#include "../../vm/vm.h"

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
      ScrWork[SW_SAVEMENUMODE] == SaveMenuPageType::QuickLoad
          ? SaveSystem::SaveType::SaveQuick
          : SaveSystem::SaveType::SaveFull;
  int SaveStatus = SaveSystem::GetSaveStatus(saveType, target->Id);
  if (SaveStatus == 1 || ScrWork[SW_SAVEMENUMODE] == SaveMenuPageType::Save) {
    ScrWork[SW_SAVEFILENO] = target->Id;
    ScrWork[SW_SAVEFILETYPE] = saveType;
    ScrWork[SW_SAVEFILESTATUS] =
        SaveSystem::GetSaveStatus(saveType, ScrWork[SW_SAVEFILENO]);

    ChoiceMade = true;
    SetFlag(1245,
            SaveSystem::GetSaveFlags(saveType, ScrWork[SW_SAVEFILENO]) & 1);
  }
}

SaveMenu::SaveMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void SaveMenu::Show() {
  auto onClick =
      std::bind(&SaveMenu::MenuButtonOnClick, this, std::placeholders::_1);

  if (State != Shown) {
    HasCleared = false;
    State = Showing;
    FadeAnimation.StartIn();
    int id = 0;
    for (int p = 0; p < Pages; ++p) {
      MainItems[p] = new Widgets::Group(this);
      MainItems[p]->WrapFocus = false;

      for (int i = 0; i < RowsPerPage; i++) {
        // Start on right col
        for (int j = EntriesPerRow - 1; j >= 0; j--) {
          Impacto::SaveSystem::SaveType saveType =
              ScrWork[SW_SAVEMENUMODE] == SaveMenuPageType::QuickLoad
                  ? SaveSystem::SaveType::SaveQuick
                  : SaveSystem::SaveType::SaveFull;
          glm::vec2 buttonPos =
              (j == 0)
                  ? glm::vec2{EntryStartXL, EntryStartYL + (i * EntryYPadding)}
                  : glm::vec2{EntryStartXR, EntryStartYR + (i * EntryYPadding)};
          SaveEntryButton* saveEntryButton = new SaveEntryButton(
              id, EntryHighlightedBoxSprite[ScrWork[SW_SAVEMENUMODE]],
              EntryHighlightedTextSprite[ScrWork[SW_SAVEMENUMODE]], p,
              buttonPos, false, SlotLockedSprite[ScrWork[SW_SAVEMENUMODE]],
              saveType, NoDataSprite[ScrWork[SW_SAVEMENUMODE]],
              BrokenDataSprite[ScrWork[SW_SAVEMENUMODE]]);

          saveEntryButton->OnClickHandler = onClick;
          id++;

          EntryGrid[p][i][j] = saveEntryButton;
          MainItems[p]->Add(saveEntryButton);
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
            int prevCol = (j + 1) % EntriesPerRow;
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
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void SaveMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems[CurrentPage]->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SaveMenu::Update(float dt) {
  UpdateInput();
  FadeAnimation.Update(dt);

  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown &&
      (ScrWork[SW_SYSSUBMENUNO] == 0 || ScrWork[SW_SYSSUBMENUNO] == 3 ||
       ScrWork[SW_SYSSUBMENUNO] == 4)) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] >= 32 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 0 || ScrWork[SW_SYSSUBMENUNO] == 3 ||
              ScrWork[SW_SYSSUBMENUNO] == 4)) {
    Show();
  }

  if (FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    int oldPage = CurrentPage;
    MainItems[CurrentPage]->Update(dt);
    CurrentPage =
        static_cast<SaveEntryButton*>(CurrentlyFocusedElement)->GetPage();
    if (CurrentPage != oldPage) {
      auto focusedElem = CurrentlyFocusedElement;
      MainItems[oldPage]->Hide();
      MainItems[CurrentPage]->Show();
      CurrentlyFocusedElement = focusedElem;
      CurrentlyFocusedElement->HasFocus = true;
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
}

void SaveMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(SaveMenuBackgroundSprite, glm::vec2(0.0f), col);
    Renderer->DrawSprite(MenuTextSprite[ScrWork[SW_SAVEMENUMODE]], {11, 10},
                         col);
    Renderer->DrawSprite(EntrySlotsSprite[ScrWork[SW_SAVEMENUMODE]], {135, 0},
                         col);

    MainItems[CurrentPage]->Tint = col;
    MainItems[CurrentPage]->Render();
    glm::vec2 pageNumPos = {
        (ScrWork[SW_SYSSUBMENUCT] * 200 * 0.0625 - 400) + 1313 + 1, 866 + 1};
    Renderer->DrawSprite(PageNumSprite[ScrWork[SW_SAVEMENUMODE]][CurrentPage],
                         pageNumPos, col);
    Renderer->DrawSprite(ButtonGuideSprite[ScrWork[SW_SAVEMENUMODE]],
                         {ScrWork[SW_SYSSUBMENUCT] * 200 * 0.0625 - 400, 989},
                         col);
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto