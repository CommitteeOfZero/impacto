#include "savemenu.h"

#include "../../profile/ui/savemenu.h"
#include "../../profile/games/chlcc/savemenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/saveentrybutton.h"
#include "../../data/savesystem.h"
#include "../../profile/game.h"
#include "../../background2d.h"
#include "../../games/chlcc/savesystem.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::SaveMenu;
using namespace Impacto::Profile::CHLCC::SaveMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::CHLCC;

Widget* EntryGrid[EntriesPerPage];

void SaveMenu::MenuButtonOnClick(Widgets::Button* target) {
  if ((SaveSystem::GetSaveStatus(EntryType, target->Id) == 1) ||
      *ActiveMenuType == +SaveMenuPageType::Save) {
    ScrWork[SW_SAVEFILENO] = target->Id;
    ScrWork[SW_SAVEFILESTATUS] =
        SaveSystem::GetSaveStatus(EntryType, ScrWork[SW_SAVEFILENO]);

    SetFlag(SF_SAVEPROTECTED,
            SaveSystem::GetSaveFlags(EntryType, ScrWork[SW_SAVEFILENO]) &
                SaveSystem::SaveFlagsMode::WriteProtect);
    ChoiceMade = true;
  }
}

SaveMenu::SaveMenu() : UI::SaveMenu() {
  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  FromSystemMenuTransition.Direction = AnimationDirection::In;
  FromSystemMenuTransition.LoopMode = AnimationLoopMode::Stop;
  FromSystemMenuTransition.DurationIn = TitleFadeInDuration;
  FromSystemMenuTransition.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  // Quick Save Pages initialization

  for (int i = 0; i < Pages; i++) {
    Widgets::Group* mainItems = new Widgets::Group(this);
    mainItems->WrapFocus = false;

    for (int j = 0; j < EntriesPerPage; j++) {
      SaveEntryButton* saveEntryButton =
          new SaveEntryButton(i * EntriesPerPage + j, QuickLoadEntrySprite,
                              EntryHighlightedSprite, QuickLoadEntrySprite,
                              EntryPositions[j], i, false, LockedSymbolSprite);
      saveEntryButton->OnClickHandler = onClick;
      saveEntryButton->AddThumbnail(EmptyThumbnailSprite, ThumbnailRelativePos);
      mainItems->Add(saveEntryButton);
      EntryGrid[j] = saveEntryButton;
    }

    // Yes I know this is bad...
    for (int k = 0; k < EntriesPerPage - 2; k++) {
      if (k % 3 == 0) {
        EntryGrid[k]->SetFocus(EntryGrid[(k % 2) + 4], FDIR_RIGHT);
        EntryGrid[k + 1]->SetFocus(EntryGrid[(k % 2) + 4], FDIR_RIGHT);
        EntryGrid[(k % 2) + 4]->SetFocus(EntryGrid[k], FDIR_LEFT);
      }
      EntryGrid[k]->SetFocus(EntryGrid[(k + 1) % 4], FDIR_DOWN);
      EntryGrid[(k + 1) % 4]->SetFocus(EntryGrid[k], FDIR_UP);
    }
    EntryGrid[4]->SetFocus(EntryGrid[5], FDIR_DOWN);
    EntryGrid[4]->SetFocus(EntryGrid[5], FDIR_UP);
    EntryGrid[5]->SetFocus(EntryGrid[4], FDIR_UP);
    EntryGrid[5]->SetFocus(EntryGrid[4], FDIR_DOWN);

    QuickSavePages.push_back(mainItems);
  }
  // Maintaining focus across pages
  for (auto pageItr = QuickSavePages.begin(); pageItr != QuickSavePages.end();
       pageItr++) {
    auto prevItr = pageItr;
    auto nextItr = pageItr;
    // Page 1 leads to page 8 to the left
    if (pageItr == QuickSavePages.begin()) {
      prevItr = QuickSavePages.end();
    }
    prevItr--;
    // Page 8 leads to page 1 to the left
    if (pageItr == --QuickSavePages.end()) {
      nextItr = QuickSavePages.begin();
    } else {
      nextItr++;
    }
    Widgets::Group* curr = *pageItr;
    Widgets::Group* prev = *prevItr;
    Widgets::Group* next = *nextItr;
    for (int k = 0; k < EntriesPerPage - 2; k++) {
      curr->Children[k]->SetFocus(prev->Children[4 + k / 2], FDIR_LEFT);
    }
    curr->Children[4]->SetFocus(next->Children[0], FDIR_RIGHT);
    curr->Children[5]->SetFocus(next->Children[3], FDIR_RIGHT);
  }

  // Full Save Pages initialization

  for (int i = 0; i < Pages; i++) {
    Widgets::Group* mainItems = new Widgets::Group(this);
    mainItems->WrapFocus = false;

    for (int j = 0; j < EntriesPerPage; j++) {
      SaveEntryButton* saveEntryButton = new SaveEntryButton(
          i * EntriesPerPage + j, SaveEntrySprite, EntryHighlightedSprite,
          SaveEntrySprite, EntryPositions[j], i, true, LockedSymbolSprite);
      saveEntryButton->OnClickHandler = onClick;
      mainItems->Add(saveEntryButton);
      EntryGrid[j] = saveEntryButton;
    }

    // Yes I know this is bad...
    for (int k = 0; k < EntriesPerPage - 2; k++) {
      if (k % 3 == 0) {
        EntryGrid[k]->SetFocus(EntryGrid[(k % 2) + 4], FDIR_RIGHT);
        EntryGrid[k + 1]->SetFocus(EntryGrid[(k % 2) + 4], FDIR_RIGHT);
        EntryGrid[(k % 2) + 4]->SetFocus(EntryGrid[k], FDIR_LEFT);
      }
      EntryGrid[k]->SetFocus(EntryGrid[(k + 1) % 4], FDIR_DOWN);
      EntryGrid[(k + 1) % 4]->SetFocus(EntryGrid[k], FDIR_UP);
    }
    EntryGrid[4]->SetFocus(EntryGrid[5], FDIR_DOWN);
    EntryGrid[4]->SetFocus(EntryGrid[5], FDIR_UP);
    EntryGrid[5]->SetFocus(EntryGrid[4], FDIR_UP);
    EntryGrid[5]->SetFocus(EntryGrid[4], FDIR_DOWN);

    FullSavePages.push_back(mainItems);
  }

  // Maintaining focus across pages
  for (auto pageItr = FullSavePages.begin(); pageItr != FullSavePages.end();
       pageItr++) {
    auto prevItr = pageItr;
    auto nextItr = pageItr;
    // Page 1 leads to page 8 to the left
    if (pageItr == FullSavePages.begin()) {
      prevItr = FullSavePages.end();
    }
    prevItr--;
    // Page 8 leads to page 1 to the right
    if (pageItr == --FullSavePages.end()) {
      nextItr = FullSavePages.begin();
    } else {
      nextItr++;
    }
    Widgets::Group* curr = *pageItr;
    Widgets::Group* prev = *prevItr;
    Widgets::Group* next = *nextItr;
    for (int k = 0; k < EntriesPerPage - 2; k++) {
      curr->Children[k]->SetFocus(prev->Children[4 + k / 2], FDIR_LEFT);
    }
    curr->Children[4]->SetFocus(next->Children[0], FDIR_RIGHT);
    curr->Children[5]->SetFocus(next->Children[3], FDIR_RIGHT);
  }
}

void SaveMenu::Init() {
  SetFlag(SF_SAVEPROTECTCHANGED, 0);

  switch (*ActiveMenuType) {
    case SaveMenuPageType::QuickLoad:
      EntryType = SaveSystem::SaveType::Quick;
      SavePages = &QuickSavePages;
      BackgroundColor = QuickLoadBackgroundColor;
      CircleSprite = QuickLoadCircle;
      MenuTitleTextSprite = QuickLoadTextSprite;
      CurrentPage = &CurrentQuickSavePage;
      break;
    case SaveMenuPageType::Save:
      EntryType = SaveSystem::SaveType::Full;
      SavePages = &FullSavePages;
      BackgroundColor = SaveBackgroundColor;
      CircleSprite = SaveCircle;
      MenuTitleTextSprite = SaveTextSprite;
      CurrentPage = &CurrentFullSavePage;
      break;
    case SaveMenuPageType::Load:
      EntryType = SaveSystem::SaveType::Full;
      SavePages = &FullSavePages;
      BackgroundColor = LoadBackgroundColor;
      CircleSprite = LoadCircle;
      MenuTitleTextSprite = LoadTextSprite;
      CurrentPage = &CurrentFullSavePage;
      break;
  }

  for (auto mainItems : *SavePages) {
    mainItems->Bounds =
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
    for (auto widget : mainItems->Children) {
      static_cast<SaveEntryButton*>(widget)->RefreshInfo(EntryType);
    }
  }
}

void SaveMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      FromSystemMenuTransition.StartIn();
      SelectAnimation.StartIn(true);
    }
    SavePages->at(*CurrentPage)->Show();
    SavePages->at(*CurrentPage)->HasFocus = false;
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    SavePages->at(*CurrentPage)->Children.front()->HasFocus = true;
    CurrentlyFocusedElement = SavePages->at(*CurrentPage)->Children.front();
  }
}
void SaveMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      SaveEntryButton::FocusedAlphaFadeReset();
      MenuTransition.StartOut();
      FromSystemMenuTransition.StartOut();
    }
    SavePages->at(*CurrentPage)->HasFocus = false;
    State = Hiding;
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
    PrevPage = *CurrentPage;
    if (CurrentlyFocusedElement) {
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement->Hovered = false;
    }
    *CurrentPage = nextPage;
    SavePages->at(*CurrentPage)->Show();
    SavePages->at(*CurrentPage)->Children.front()->HasFocus = true;
    CurrentlyFocusedElement = SavePages->at(*CurrentPage)->Children.front();
  };
  if (IsFocused) {
    SavePages->at(*CurrentPage)->UpdateInput(dt);
    if (Input::MouseWheelDeltaY < 0 || PADinputButtonWentDown & PADcustom[8]) {
      updatePage((*CurrentPage + 1) % Pages);
    } else if (Input::MouseWheelDeltaY > 0 ||
               PADinputButtonWentDown & PADcustom[7]) {
      updatePage((*CurrentPage - 1 + Pages) % Pages);
    }

    if (CurrentlyFocusedElement && (PADinputButtonWentDown & PAD1Y)) {
      auto saveButton = static_cast<SaveEntryButton*>(CurrentlyFocusedElement);
      if (SaveSystem::GetSaveStatus(EntryType, saveButton->Id) == 1) {
        saveButton->ToggleLock(EntryType);
        saveButton->RefreshInfo(EntryType);
        SetFlag(SF_SAVEPROTECTCHANGED, 1);
      }
    }
  }
}

void SaveMenu::Update(float dt) {
  const int sysMenuCt = ScrWork[SW_SYSMENUCT];
  const int systemMenuCHG = ScrWork[SW_SYSTEMMENUCHG];

  if ((!GetFlag(SF_SAVEMENU) || sysMenuCt < 10000 ||
       (sysMenuCt == 10000 && systemMenuCHG != 0 && systemMenuCHG != 64)) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_SAVEMENU) && sysMenuCt > 0 && State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && !GetFlag(SF_SAVEMENU) && systemMenuCHG == 0 &&
      (sysMenuCt == 0 || GetFlag(SF_SYSTEMMENU)) && State == Hiding) {
    State = Hidden;
    SavePages->at(*CurrentPage)->Hide();
  } else if (MenuTransition.IsIn() && sysMenuCt == 10000 &&
             (systemMenuCHG == 0 || systemMenuCHG == 64) &&
             GetFlag(SF_SAVEMENU) && State == Showing) {
    State = Shown;
    SavePages->at(*CurrentPage)->HasFocus = true;
    SaveEntryButton::FocusedAlphaFadeStart();
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    SelectAnimation.Update(dt);
    FromSystemMenuTransition.Update(dt);
    if (MenuTransition.Direction == AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    UpdateTitles();
    if (IsFocused) {
      SavePages->at(*CurrentPage)->Update(dt);
      SaveEntryButton::UpdateFocusedAlphaFade(dt);
      auto currentlyFocusedButton =
          static_cast<SaveEntryButton*>(CurrentlyFocusedElement);
      if (currentlyFocusedButton) {
        int newPage = currentlyFocusedButton->GetPage();
        if (newPage != *CurrentPage) {
          SavePages->at(*CurrentPage)->Hide();
          *CurrentPage = newPage;
          SavePages->at(*CurrentPage)->Show();
          currentlyFocusedButton->HasFocus = true;
          CurrentlyFocusedElement = currentlyFocusedButton;
        }
      }
    } else {
      // We want to keep the fade even when the confirmation prompt appears
      SaveEntryButton::UpdateFocusedAlphaFade(dt);
    }
  }
  if (State == Shown) {
    UpdateInput(dt);
  }
}

void SaveMenu::Render() {
  if (State != Hidden && ScrWork[SW_FILEALPHA] != 0) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawQuad(
          RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
          RgbIntToFloat(BackgroundColor));
    } else if (GetFlag(SF_SYSTEMMENU)) {
      Renderer->DrawQuad(
          RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
          RgbIntToFloat(BackgroundColor, FromSystemMenuTransition.Progress));
    } else {
      DrawCircles();
    }
    DrawErin();
    DrawRedBar();

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

      const CornersQuad titleDest = MenuTitleTextSprite.ScaledBounds()
                                        .RotateAroundCenter(MenuTitleTextAngle)
                                        .Translate(RightTitlePos);
      Renderer->DrawSprite(MenuTitleTextSprite, titleDest);
    }

    Renderer->CaptureScreencap(ShaderScreencapture.BgSprite);
    Renderer->DrawCHLCCMenuBackground(
        ShaderScreencapture.BgSprite, BackgroundFilter,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        MenuTransition.Progress);

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(MenuTitleTextSprite, LeftTitlePos);
    }

    if (MenuTransition.Progress < 0.22f) return;
    glm::vec2 offset = MenuTransition.GetPageOffset();

    SavePages->at(*CurrentPage)->MoveTo(offset);
    SavePages->at(*CurrentPage)->Render();
    Renderer->DrawSprite(SaveListSprite, SaveListPosition + offset);
    DrawPageNumber(offset.y);
    DrawButtonPrompt();
    SelectAnimation.Draw(SelectDataTextSprites, SelectDataTextPositions,
                         offset);
  }
}

inline void SaveMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * FadeInDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        Renderer->DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
};

inline void SaveMenu::DrawErin() {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = 801.0f;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void SaveMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * FadeInDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}

inline void SaveMenu::DrawPageNumber(float yOffset) {
  Renderer->DrawSprite(
      PageNumBackgroundSprite,
      glm::vec2(PageNumBackgroundPos.x, PageNumBackgroundPos.y + yOffset));
  Renderer->DrawSprite(
      BigDigits[*CurrentPage + 1],
      glm::vec2(CurrentPageNumPos.x, CurrentPageNumPos.y + yOffset));
  Renderer->DrawSprite(PageNumSeparatorSlashSprite,
                       glm::vec2(PageNumSeparatorSlashPos.x,
                                 PageNumSeparatorSlashPos.y + yOffset));
  Renderer->DrawSprite(MaxPageNumSprite,
                       glm::vec2(MaxPageNumPos.x, MaxPageNumPos.y + yOffset));
}

inline void SaveMenu::DrawButtonPrompt() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (MenuTransition.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (MenuTransition.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
  }
}

void SaveMenu::UpdateTitles() {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos =
      MenuTitleTextRightPos -
      glm::vec2(0.0f, (MenuTitleTextSprite.Bounds.Height + 2.0f) / 2.0f);
  LeftTitlePos = glm::vec2(
      MenuTitleTextLeftPos.x,
      TitleFade.IsIn()
          ? MenuTitleTextLeftPos.y
          : glm::mix(
                1.0f, 721.0f,
                1.01011f * std::sin(1.62223f * TitleFade.Progress + 3.152f) +
                    1.01012f));

  if (MenuTransition.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
}

void SaveMenu::RefreshCurrentEntryInfo() {
  if (!CurrentlyFocusedElement) return;
  static_cast<SaveEntryButton*>(CurrentlyFocusedElement)
      ->RefreshInfo(EntryType);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto