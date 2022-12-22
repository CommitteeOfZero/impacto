#include "tipsmenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/ui/tipsmenu.h"
#include "../../profile/games/mo6tw/tipsmenu.h"
#include "../../ui/widgets/mo6tw/tipsentrybutton.h"
#include "../../io/memorystream.h"
#include "../../data/tipssystem.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::TipsMenu;
using namespace Impacto::Profile::MO6TW::TipsMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::MO6TW;

void TipsMenu::TipOnClick(Button *target) {
  auto tipEntry = static_cast<TipsEntryButton *>(target);
  if (!tipEntry->TipEntryRecord->IsLocked) SwitchToTipId(target->Id);
}

TipsMenu::TipsMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  TipViewItems = new Group(this);
  TipViewItems->FocusLock = false;
  TipViewItems->IsShown = true;

  Name = new Label();
  Name->Bounds = NameInitialBounds;
  TipViewItems->Add(Name);

  Pronounciation = new Label();
  Pronounciation->Bounds = PronounciationInitialBounds;
  TipViewItems->Add(Pronounciation);

  Category = new Label();
  Category->Bounds = CategoryInitialBounds;
  TipViewItems->Add(Category);

  TextPage = new DialoguePage();
  TextPage->Glyphs = new ProcessedTextGlyph[Profile::Dialogue::MaxPageSize];
  TextPage->Clear();
  TextPage->Mode = DPM_TIPS;
  TextPage->FadeAnimation.Progress = 1.0f;
}

void TipsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void TipsMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
    CurrentlyDisplayedTipId = -1;
  }
}

void TipsMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    ItemsList->UpdateInput();
    if (CurrentlyDisplayedTipId != -1) {
      if (PADinputButtonWentDown & PAD1X) {
        NextTipPage();
      }
    }
  }
}

void TipsMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_TIPSALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_TIPSALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_TIPSALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_TIPSALPHA] == 0 && FadeAnimation.IsOut())
    State = Hidden;

  ItemsList->Tint.a = FadeAnimation.Progress;
  TipViewItems->Tint.a = FadeAnimation.Progress;

  if (State == Shown) {
    ItemsList->Update(dt);
    TipViewItems->Update(dt);
  }
}

void TipsMenu::Render() {
  if (State != Hidden && ScrWork[SW_TIPSALPHA] > 0) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f), col);
    ItemsList->Render();

    if (CurrentlyDisplayedTipId != -1) {
      TipViewItems->Render();
      Renderer->DrawProcessedText(TextPage->Glyphs, TextPage->Length,
                                    Profile::Dialogue::DialogueFont, col.a,
                                    true, true);
      if (ThumbnailSprite) {
        Renderer->DrawSprite(*ThumbnailSprite, ThumbnailPosition, col);
      }
    }
  }
}

void TipsMenu::Init() {
  ItemsList = new Carousel(CDIR_HORIZONTAL);
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto onClick = std::bind(&TipsMenu::TipOnClick, this, std::placeholders::_1);
  int idx = 0, currentPage = 0, currentCategoryId = -1;
  Group *pageItems = new Group(this);

  // String of characters by which tips are sorted, taken from _system script
  auto sortString = (uint16_t *)Vm::ScriptGetTextTableStrAddress(
      SortStringTable, SortStringIndex);

  Records = TipsSystem::GetTipRecords();
  auto recordCount = TipsSystem::GetTipCount();

  float currentY = TipListInitialY;

  for (int i = 0; i < recordCount; i++) {
    auto record = &Records[i];

    // In this case the tips are split into pages in UI with maximum of 5
    // categories per page, with each category being a character from
    // the sort string and containing all tips the names of which begin with
    // that character
    auto page = record->SortLetterIndex / TipListCategoriesPerPage;
    if (page != currentPage) {
      if (currentPage == 0) pageItems->Show();
      currentPage = page;
      currentY = TipListInitialY;
      ItemsList->Add(pageItems);
      pageItems = new Group(this);
    }

    // Start new category
    // We take a character from the sort string and use that as the category
    // name inside a predefined template
    if (record->SortLetterIndex != currentCategoryId) {
      currentCategoryId = record->SortLetterIndex;
      CategoryString[1] = sortString[currentCategoryId];

      Label *categoryLabel = new Label();
      categoryLabel->Bounds.X = TipListEntryBounds.X;
      categoryLabel->Bounds.Y = currentY;
      categoryLabel->SetText((uint8_t *)CategoryString, TipListEntryFontSize,
                             true, 0);
      pageItems->Add(categoryLabel);
      currentY += TipListYPadding;
    }

    // Actual tip entry button
    TipListEntryBounds.Y = currentY;
    TipsEntryButton *button =
        new TipsEntryButton(record->Id, record, TipListEntryBounds,
                            Profile::BacklogMenu::EntryHighlight);
    button->OnClickHandler = onClick;

    pageItems->Add(button, FDIR_DOWN);
    currentY += TipListYPadding;
  }

  // Add last category
  ItemsList->Add(pageItems);

  // Number label
  NumberText = new Label(Vm::ScriptGetTextTableStrAddress(NumberLabelStrTable,
                                                          NumberLabelStrIndex),
                         NumberLabelPosition, NumberLabelFontSize, true,
                         DefaultColorIndex);
  TipViewItems->Add(NumberText);
  // Tip number
  Number = new Label();
  Number->Bounds = NumberBounds;
  TipViewItems->Add(Number);
  // Tip page separator
  PageSeparator = new Label(
      Vm::ScriptGetTextTableStrAddress(PageSeparatorTable, PageSeparatorIndex),
      PageSeparatorPosition, PageSeparatorFontSize, true, DefaultColorIndex);
  TipViewItems->Add(PageSeparator);
  // Current tip page
  CurrentPage = new Label();
  CurrentPage->Bounds = CurrentPageBounds;
  TipViewItems->Add(CurrentPage);
  // Total tip pages
  TotalPages = new Label();
  TotalPages->Bounds = TotalPagesBounds;
  TipViewItems->Add(TotalPages);
}

void TipsMenu::SwitchToTipId(int id) {
  CurrentlyDisplayedTipId = id;
  CurrentTipPage = 1;
  TipsSystem::SetTipUnreadState(id, false);
  TipsSystem::SetTipNewState(id, false);

  auto tipRecord = &Records[id];
  Name->SetText(tipRecord->StringPtrs[0], NameFontSize, true,
                DefaultColorIndex);
  Pronounciation->SetText(tipRecord->StringPtrs[1], PronounciationFontSize,
                          true, DefaultColorIndex);

  Vm::Sc3VmThread dummy;
  dummy.Ip = tipRecord->StringPtrs[2];
  float categoryWidth = TextGetPlainLineWidth(
      &dummy, Profile::Dialogue::DialogueFont, CategoryFontSize);
  Category->Bounds.X = CategoryEndX - categoryWidth;
  Category->SetText(tipRecord->StringPtrs[2], CategoryFontSize, true,
                    DefaultColorIndex);
  if (tipRecord->ThumbnailIndex != 0xFFFF)
    ThumbnailSprite = &TipThumbnails[tipRecord->ThumbnailIndex];
  else
    ThumbnailSprite = &TipTextOnlyThumbnail;

  char temp[5];
  sprintf(temp, "%4d", tipRecord->Id + 1);
  Number->SetText(std::string(temp), NumberFontSize, true, DefaultColorIndex);

  sprintf(temp, "%d", CurrentTipPage);
  CurrentPage->SetText(std::string(temp), PageSeparatorFontSize, true,
                       DefaultColorIndex);

  sprintf(temp, "%d", tipRecord->NumberOfContentStrings);
  TotalPages->SetText(std::string(temp), PageSeparatorFontSize, true,
                      DefaultColorIndex);

  TextPage->Clear();
  dummy.Ip = tipRecord->StringPtrs[3];
  TextPage->AddString(&dummy);
}

void TipsMenu::NextTipPage() {
  CurrentTipPage += 1;
  if (CurrentTipPage > Records[CurrentlyDisplayedTipId].NumberOfContentStrings)
    CurrentTipPage = 1;

  TextPage->Clear();
  Vm::Sc3VmThread dummy;
  dummy.Ip = Records[CurrentlyDisplayedTipId].StringPtrs[2 + CurrentTipPage];
  TextPage->AddString(&dummy);

  char temp[5];
  sprintf(temp, "%d", CurrentTipPage);
  CurrentPage->SetText(std::string(temp), PageSeparatorFontSize, true,
                       DefaultColorIndex);
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto