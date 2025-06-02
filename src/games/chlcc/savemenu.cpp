#include "savemenu.h"

#include <iomanip>
#include <sstream>

#include "../../profile/ui/savemenu.h"
#include "../../profile/games/chlcc/savemenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/saveentrybutton.h"
#include "../../data/savesystem.h"
#include "../../vm/vm.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::SaveMenu;
using namespace Impacto::Profile::CHLCC::SaveMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::CHLCC;

Widget* EntryGrid[EntriesPerPage];

void SaveMenu::MenuButtonOnClick(Widgets::Button* target) {
  if ((SaveSystem::GetSaveStatus(EntryType, target->Id) != 0) ||
      ActiveMenuType == +SaveMenuPageType::Save) {
    ScrWork[SW_SAVEFILENO] = target->Id;
    ChoiceMade = true;
  }
}

SaveMenu::SaveMenu() {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.DurationIn = FadeInDuration;
  MenuTransition.DurationOut = FadeOutDuration;

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  SelectDataTextFade.Direction = AnimationDirection::In;
  SelectDataTextFade.LoopMode = AnimationLoopMode::Loop;
  SelectDataTextFade.DurationIn = SelectDataFadeDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  // Quick Save Pages initialization

  for (int i = 0; i < Pages; i++) {
    MainItems = new Widgets::Group(this);
    MainItems->WrapFocus = false;

    for (int j = 0; j < EntriesPerPage; j++) {
      SaveEntryButton* saveEntryButton =
          new SaveEntryButton(i * EntriesPerPage + j, QuickLoadEntrySprite,
                              EntryHighlightedSprite, QuickLoadEntrySprite,
                              EntryPositions[j], i, false, LockedSymbolSprite);
      saveEntryButton->OnClickHandler = onClick;
      saveEntryButton->AddThumbnail(EmptyThumbnailSprite, ThumbnailRelativePos);
      MainItems->Add(saveEntryButton);
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

    QuickSavePages.push_back(MainItems);
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
    MainItems = new Widgets::Group(this);
    MainItems->WrapFocus = false;

    for (int j = 0; j < EntriesPerPage; j++) {
      SaveEntryButton* saveEntryButton = new SaveEntryButton(
          i * EntriesPerPage + j, SaveEntrySprite, EntryHighlightedSprite,
          SaveEntrySprite, EntryPositions[j], i, true, LockedSymbolSprite);
      saveEntryButton->OnClickHandler = onClick;
      MainItems->Add(saveEntryButton);
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

    FullSavePages.push_back(MainItems);
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

void SaveMenu::UpdateEntry(SaveEntryButton* saveEntryButton) {
  Sprite entrySprite;
  switch (*UI::SaveMenuPtr->ActiveMenuType) {
    case SaveMenuPageType::QuickLoad:
      entrySprite = QuickLoadEntrySprite;
      break;
    case SaveMenuPageType::Save:
      entrySprite = SaveEntrySprite;
      break;
    case SaveMenuPageType::Load:
      entrySprite = LoadEntrySprite;
      break;
  }
  int idx = saveEntryButton->Id;
  uint8_t lock = SaveSystem::GetSaveFlags(EntryType, idx);
  saveEntryButton->IsLocked = lock == 1;
  saveEntryButton->AddNormalSpriteLabel(entrySprite, EntryPositions[idx % 6]);
  saveEntryButton->AddEntryNumberHintText(
      Vm::ScriptGetTextTableStrAddress(0, 6), 18,
      RendererOutlineMode::BottomRight, EntryNumberHintTextRelativePos);
  saveEntryButton->AddEntryNumberText(fmt::format("{:02}", idx + 1), 18,
                                      RendererOutlineMode::BottomRight,
                                      EntryNumberTextRelativePos);
  saveEntryButton->AddThumbnail(EmptyThumbnailSprite,
                                EntryPositions[idx % 6] + ThumbnailRelativePos);
  if (SaveSystem::GetSaveStatus(EntryType, idx) != 0) {
    saveEntryButton->EntryActive = true;
    saveEntryButton->AddSceneTitleText(
        Vm::ScriptGetTextTableStrAddress(
            1, SaveSystem::GetSaveTitle(EntryType, idx)),
        24, RendererOutlineMode::BottomRight, SceneTitleTextRelativePos,
        NoDataTextRelativePos);
    saveEntryButton->AddPlayTimeHintText(Vm::ScriptGetTextTableStrAddress(0, 2),
                                         18, RendererOutlineMode::BottomRight,
                                         PlayTimeHintTextRelativePos);
    uint32_t time = SaveSystem::GetSavePlayTime(EntryType, idx);
    uint32_t hours = time / 3600;
    uint32_t minutes = (time % 3600) / 60;
    uint32_t seconds = (time % 3600) % 60;
    saveEntryButton->AddPlayTimeText(
        fmt::format("{:3}:{:02}:{:02}", hours, minutes, seconds), 18,
        RendererOutlineMode::BottomRight,
        {PlayTimeTextRelativePos.x + (float)((hours < 10) * 10),
         PlayTimeTextRelativePos.y});
    saveEntryButton->AddSaveDateHintText(Vm::ScriptGetTextTableStrAddress(0, 3),
                                         18, RendererOutlineMode::BottomRight,
                                         SaveDateHintTextRelativePos);
    std::stringstream dateStr;
    tm const& date = SaveSystem::GetSaveDate(EntryType, idx);
    dateStr << std::put_time(&date, "  %y/%m/%d %H:%M:%S");
    saveEntryButton->AddSaveDateText(dateStr.str(), 18,
                                     RendererOutlineMode::BottomRight,
                                     SaveDateTextRelativePos);
  } else {
    saveEntryButton->AddSceneTitleText(Vm::ScriptGetTextTableStrAddress(0, 1),
                                       24, RendererOutlineMode::BottomRight,
                                       SceneTitleTextRelativePos,
                                       NoDataTextRelativePos);
  }
}

void SaveMenu::Show() {
  if (State != Shown) {
    switch (*ActiveMenuType) {
      case SaveMenuPageType::QuickLoad:
        EntryType = SaveSystem::SaveQuick;
        SavePages = &QuickSavePages;
        BackgroundColor = QuickLoadBackgroundColor;
        CircleSprite = QuickLoadCircle;
        MenuTitleTextSprite = QuickLoadTextSprite;
        CurrentPage = &CurrentQuickSavePage;
        break;
      case SaveMenuPageType::Save:
        EntryType = SaveSystem::SaveFull;
        SavePages = &FullSavePages;
        BackgroundColor = SaveBackgroundColor;
        CircleSprite = SaveCircle;
        MenuTitleTextSprite = SaveTextSprite;
        CurrentPage = &CurrentFullSavePage;
        break;
      case SaveMenuPageType::Load:
        EntryType = SaveSystem::SaveFull;
        SavePages = &FullSavePages;
        BackgroundColor = LoadBackgroundColor;
        CircleSprite = LoadCircle;
        MenuTitleTextSprite = LoadTextSprite;
        CurrentPage = &CurrentFullSavePage;
        break;
    }

    for (auto mainItems : *SavePages) {
      mainItems->Bounds = RectF(0.0f, 0.0f, 1280.0f, 720.0f);
      for (auto widget : mainItems->Children) {
        auto saveEntryButton = static_cast<SaveEntryButton*>(widget);
        UpdateEntry(saveEntryButton);
      }
    }

    State = Showing;
    MenuTransition.StartIn();
    SelectDataTextFade.StartIn();
    SavePages->at(*CurrentPage)->Show();
    CurrentlyFocusedElement = SavePages->at(*CurrentPage)->Children[0];
    SavePages->at(0)->Children[0]->HasFocus = true;
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
    SaveEntryButton::FocusedAlphaFadeReset();
    MenuTransition.StartOut();
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
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_SAVEMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (ScrWork[SW_SYSMENUCT] == 0 && State == Hiding) {
    State = Hidden;
    SavePages->at(*CurrentPage)->Hide();
  } else if (ScrWork[SW_SYSMENUCT] == 10000 && State == Showing) {
    State = Shown;
    SaveEntryButton::FocusedAlphaFadeStart();
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    SelectDataTextFade.Update(dt);
    if (MenuTransition.Direction == +AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == +AnimationDirection::In ||
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
}

void SaveMenu::Render() {
  if (State != Hidden && ScrWork[SW_FILEALPHA] != 0) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawQuad(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
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
        RectF(0.0f, 0.0f, 1280.0f, 720.0f), MenuTransition.Progress);

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(MenuTitleTextSprite, LeftTitlePos);
    }

    float yOffset = 0;
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.73f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -720.0f, 0.0f,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }
      SavePages->at(*CurrentPage)->MoveTo(glm::vec2(0, yOffset));
      SavePages->at(*CurrentPage)->Render();
      Renderer->DrawSprite(SaveListSprite,
                           SaveListPosition + glm::vec2(0, yOffset));
      DrawPageNumber(yOffset);
      DrawButtonPrompt();
      DrawSelectData(yOffset);
    }
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

inline void SaveMenu::DrawSelectData(float yOffset) {
  float alpha;
  for (int idx = 0; idx < 10; idx++) {
    alpha = 1.0f;
    if (SelectDataTextFade.Progress < 0.046f * (idx + 1)) {
      alpha = (SelectDataTextFade.Progress - 0.046f * idx) / 0.046f;
    }
    Renderer->DrawSprite(SelectDataTextSprites[idx],
                         glm::vec2(SelectDataTextPositions[idx].x,
                                   SelectDataTextPositions[idx].y + yOffset),
                         glm::vec4(glm::vec3(1.0f), alpha));
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

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto