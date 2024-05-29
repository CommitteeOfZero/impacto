#include "savemenu.h"

#include "../../profile/ui/savemenu.h"
#include "../../profile/games/cclcc/savemenu.h"
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

Widget* EntryGrid[RowsPerPage][EntriesPerRow];

void SaveMenu::MenuButtonOnClick(Widgets::Button* target) {
  if ((SaveSystem::GetSaveSatus(SaveSystem::SaveType::SaveFull, target->Id) !=
       0) ||
      ScrWork[SW_SAVEMENUMODE] == 1) {
    ScrWork[SW_SAVEFILENO] = target->Id;
    ChoiceMade = true;
  }
}

SaveMenu::SaveMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void SaveMenu::Show() {
  if (State != Shown) {
    MainItems = new Widgets::Group(this);
    MainItems->WrapFocus = false;

    auto onClick =
        std::bind(&SaveMenu::MenuButtonOnClick, this, std::placeholders::_1);

    int id = 0;
    for (int p = 0; p < Pages; ++p) {
      for (int i = 0; i < RowsPerPage; i++) {
        for (int j = 0; j < EntriesPerRow; j++) {
          glm::vec2 buttonPos =
              (j == 0)
                  ? glm::vec2{EntryStartXL, EntryStartYL + (i * EntryYPadding)}
                  : glm::vec2{EntryStartXR, EntryStartYR + (i * EntryYPadding)};
          SaveEntryButton* saveEntryButton = new SaveEntryButton(
              id, EntryHighlightedBoxSprite[ScrWork[SW_SAVEMENUMODE]],
              EntryHighlightedTextSprite[ScrWork[SW_SAVEMENUMODE]], buttonPos,
              false, SlotLockedSprite[ScrWork[SW_SAVEMENUMODE]]);

          saveEntryButton->OnClickHandler = onClick;
          // if (SaveSystem::GetSaveSatus(SaveSystem::SaveType::SaveFull, id) !=
          // 0) { saveEntryButton->EntryActive = true;
          // saveEntryButton->AddSceneTitleText(
          //     Vm::ScriptGetTextTableStrAddress(
          //         1,
          //         SaveSystem::GetSaveTitle(SaveSystem::SaveType::SaveFull,
          //         id)),
          //     20, RO_BottomRight, );
          // saveEntryButton->AddPlayTimeHintText(
          //     Vm::ScriptGetTextTableStrAddress(0, 2), 16, true);
          // saveEntryButton->AddPlayTimeText(
          //     Vm::ScriptGetTextTableStrAddress(0, 15), 16, true);
          // saveEntryButton->AddSaveDateHintText(
          //     Vm::ScriptGetTextTableStrAddress(0, 3), 16, true);
          // saveEntryButton->AddSaveDateText(
          //     Vm::ScriptGetTextTableStrAddress(0, 14), 16, true);
          // } else {
          // saveEntryButton->AddSceneTitleText(
          //     Vm::ScriptGetTextTableStrAddress(0, 1), 24, true);
          // }
          // saveEntryButton->Thumbnail = EmptyThumbnailSprite;
          id++;
          if (j == EntriesPerRow - 1) {
            MainItems->Add(saveEntryButton, FDIR_RIGHT);
          } else {
            MainItems->Add(saveEntryButton);
          }
          EntryGrid[i][j] = saveEntryButton;
        }
      }
      for (int j = 0; j < EntriesPerRow; j++) {
        for (int i = 0; i < RowsPerPage; i++) {
          if (i != RowsPerPage - 1) {
            EntryGrid[i][j]->SetFocus(EntryGrid[i + 1][j], FDIR_DOWN);
            EntryGrid[i + 1][j]->SetFocus(EntryGrid[i][j], FDIR_UP);
          }
          if (j != EntriesPerRow - 1) {
            EntryGrid[i][j]->SetFocus(EntryGrid[i][j + 1], FDIR_RIGHT);
            EntryGrid[i][j + 1]->SetFocus(EntryGrid[i][j], FDIR_LEFT);
          }
        }
      }
    }

    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    CurrentlyFocusedElement = EntryGrid[0][0];
    EntryGrid[0][0]->HasFocus = true;
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

void SaveMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);

  if (ScrWork[2147] < 32 && State == Shown &&
      (ScrWork[2148] == 0 || ScrWork[2148] == 3 || ScrWork[2148] == 4)) {
    Hide();
  } else if (ScrWork[2147] >= 32 && State == Hidden &&
             (ScrWork[2148] == 0 || ScrWork[2148] == 3 || ScrWork[2148] == 4)) {
    Show();
  }

  if (FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
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

    MainItems->Tint = col;
    MainItems->Render();
    Renderer->DrawSprite(ButtonGuideSprite[ScrWork[SW_SAVEMENUMODE]],
                         {ScrWork[2147] * 200 * 0.0625 - 400, 989}, col);
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto