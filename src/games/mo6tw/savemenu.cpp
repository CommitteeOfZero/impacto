#include "savemenu.h"

#include "../../profile/ui/savemenu.h"
#include "../../profile/games/mo6tw/savemenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/mo6tw/saveentrybutton.h"
#include "../../data/savesystem.h"
#include "../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::SaveMenu;
using namespace Impacto::Profile::MO6TW::SaveMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::MO6TW;

Widget* EntryGrid[RowsPerPage][EntriesPerRow];

void SaveMenu::MenuButtonOnClick(Widgets::Button* target) {
  if ((SaveSystem::GetSaveStatus(SaveSystem::SaveType::Full, target->Id) !=
       0) ||
      *ActiveMenuType == +SaveMenuPageType::Save) {
    ScrWork[SW_SAVEFILENO] = target->Id;
    ChoiceMade = true;
  }
}

SaveMenu::SaveMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void SaveMenu::Show() {
  if (State != Shown) {
    MainItems = new Widgets::Group(this);
    MainItems->WrapFocus = false;

    auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

    Sprite entrySprite;
    Sprite entryHSprite;
    Sprite nullSprite = Sprite();
    nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

    switch (*ActiveMenuType) {
      case SaveMenuPageType::QuickLoad:
        entrySprite = QuickLoadEntrySprite;
        entryHSprite = QuickLoadEntryHighlightedSprite;
        break;
      case SaveMenuPageType::Save:
        entrySprite = SaveEntrySprite;
        entryHSprite = SaveEntryHighlightedSprite;
        break;
      case SaveMenuPageType::Load:
        entrySprite = LoadEntrySprite;
        entryHSprite = LoadEntryHighlightedSprite;
        break;
    }
    int id = 0;
    for (int i = 0; i < RowsPerPage; i++) {
      for (int j = 0; j < EntriesPerRow; j++) {
        SaveEntryButton* saveEntryButton =
            new SaveEntryButton(id, entrySprite, entryHSprite, nullSprite,
                                glm::vec2(EntryStartX + (j * EntryXPadding),
                                          EntryStartY + (i * EntryYPadding)));

        saveEntryButton->OnClickHandler = onClick;
        saveEntryButton->DisabledSprite = entrySprite;
        if (SaveSystem::GetSaveStatus(SaveSystem::SaveType::Full, id) != 0) {
          saveEntryButton->EntryActive = true;
          saveEntryButton->AddSceneTitleText(
              Vm::ScriptGetTextTableStrAddress(
                  1, SaveSystem::GetSaveTitle(SaveSystem::SaveType::Full, id)),
              20, true);
          saveEntryButton->AddPlayTimeHintText(
              Vm::ScriptGetTextTableStrAddress(0, 2), 16, true);
          saveEntryButton->AddPlayTimeText(
              Vm::ScriptGetTextTableStrAddress(0, 15), 16, true);
          saveEntryButton->AddSaveDateHintText(
              Vm::ScriptGetTextTableStrAddress(0, 3), 16, true);
          saveEntryButton->AddSaveDateText(
              Vm::ScriptGetTextTableStrAddress(0, 14), 16, true);
        } else {
          saveEntryButton->AddSceneTitleText(
              Vm::ScriptGetTextTableStrAddress(0, 1), 24, true);
        }
        saveEntryButton->Thumbnail = EmptyThumbnailSprite;
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
  UpdateInput(dt);

  FadeAnimation.Update(dt);
  if (ScrWork[SW_FILEALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_FILEALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_FILEALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_FILEALPHA] == 0 && FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}

void SaveMenu::Render() {
  if (State != Hidden && ActiveMenuType) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(SaveMenuBackgroundSprite, glm::vec2(0.0f), col);
    switch (*ActiveMenuType) {
      case SaveMenuPageType::QuickLoad:
        Renderer->DrawSprite(QuickLoadTextSprite, MenuTitleTextPos, col);
        break;
      case SaveMenuPageType::Save:
        Renderer->DrawSprite(SaveTextSprite, MenuTitleTextPos, col);
        break;
      case SaveMenuPageType::Load:
        Renderer->DrawSprite(LoadTextSprite, MenuTitleTextPos, col);
        break;
    }
    MainItems->Tint = col;
    MainItems->Render();
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto