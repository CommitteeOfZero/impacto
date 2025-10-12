#include "tipsmenu.h"
#include <fmt/format.h>

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/ui/tipsmenu.h"
#include "../../profile/games/mo6tw/tipsmenu.h"
#include "../../ui/widgets/mo6tw/tipsentrybutton.h"
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

TipsMenu::TipsMenu() : ItemsList(CDIR_HORIZONTAL), TipViewItems(this) {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  TipViewItems.FocusLock = false;
  TipViewItems.IsShown = true;

  Name = new Label();
  Name->Bounds = NameInitialBounds;
  TipViewItems.Add(Name);

  Pronounciation = new Label();
  Pronounciation->Bounds = PronounciationInitialBounds;
  TipViewItems.Add(Pronounciation);

  Category = new Label();
  Category->Bounds = CategoryInitialBounds;
  TipViewItems.Add(Category);

  TextPage.Glyphs.reserve(Profile::Dialogue::MaxPageSize);
  TextPage.Clear();
  TextPage.Mode = DPM_TIPS;
  TextPage.FadeAnimation.Progress = 1.0f;
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

void TipsMenu::UpdateInput(float dt) {
  Menu::UpdateInput(dt);
  if (State == Shown) {
    ItemsList.UpdateInput(dt);
    if (CurrentlyDisplayedTipId != -1) {
      if (PADinputButtonWentDown & PAD1X) {
        NextTipPage();
      }
    }
  }
}

void TipsMenu::Update(float dt) {
  UpdateInput(dt);

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

  if (State != Hidden) {
    ItemsList.Tint.a = FadeAnimation.Progress;
    TipViewItems.Tint.a = FadeAnimation.Progress;
  }

  if (State == Shown) {
    ItemsList.Update(dt);
    TipViewItems.Update(dt);
  }
}

void TipsMenu::Render() {
  if (State != Hidden && ScrWork[SW_TIPSALPHA] > 0) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f), col);
    ItemsList.Render();

    if (CurrentlyDisplayedTipId != -1) {
      TipViewItems.Render();
      Renderer->DrawProcessedText(TextPage.Glyphs,
                                  Profile::Dialogue::DialogueFont, col.a,
                                  RendererOutlineMode::Full, true);
      if (ThumbnailSprite) {
        Renderer->DrawSprite(*ThumbnailSprite, ThumbnailPosition, col);
      }
    }
  }
}

void TipsMenu::Init() {
  auto onClick = [this](auto *btn) { return TipOnClick(btn); };
  int currentPage = 0, currentCategoryId = -1;

  // String of characters by which tips are sorted, taken from _system script
  auto [scriptBufId, sortStrAddr] =
      Vm::ScriptGetTextTableStrAddress(SortStringTable, SortStringIndex);
  uint8_t *sortString = &Vm::ScriptBuffers[scriptBufId][sortStrAddr];
  auto recordCount = TipsSystem::GetTipCount();

  float currentY = TipListInitialY;

  ItemsList.Clear();
  TipViewItems.Clear();

  Group *pageItems = new Group(this);
  for (size_t i = 0; i < recordCount; i++) {
    auto record = TipsSystem::GetTipRecord(i);

    // In this case the tips are split into pages in UI with maximum of 5
    // categories per page, with each category being a character from
    // the sort string and containing all tips the names of which begin with
    // that character
    auto page = record->CategoryLetterIndex / TipListCategoriesPerPage;
    if (page != currentPage) {
      if (currentPage == 0) pageItems->Show();
      currentPage = page;
      currentY = TipListInitialY;
      ItemsList.Add(pageItems);
      pageItems = new Group(this);
    }

    // Start new category
    // We take a character from the sort string and use that as the category
    // name inside a predefined template
    if (record->CategoryLetterIndex != currentCategoryId) {
      currentCategoryId = record->CategoryLetterIndex;
      CategoryString[1] = UnalignedRead<uint16_t>(
          &sortString[currentCategoryId * sizeof(uint16_t)]);

      Label *categoryLabel = new Label();
      categoryLabel->Bounds.X = TipListEntryBounds.X;
      categoryLabel->Bounds.Y = currentY;
      Vm::Sc3Stream categoryStrStream(
          reinterpret_cast<uint8_t *>(CategoryString));
      categoryLabel->SetText(categoryStrStream, TipListEntryFontSize,
                             RendererOutlineMode::Full, 0);
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
  ItemsList.Add(pageItems);

  // Number label
  NumberText = new Label(Vm::ScriptGetTextTableStrAddress(NumberLabelStrTable,
                                                          NumberLabelStrIndex),
                         NumberLabelPosition, NumberLabelFontSize,
                         RendererOutlineMode::Full, DefaultColorIndex);
  TipViewItems.Add(NumberText);
  // Tip number
  Number = new Label();
  Number->Bounds = NumberBounds;
  TipViewItems.Add(Number);
  // Tip page separator
  PageSeparator = new Label(
      Vm::ScriptGetTextTableStrAddress(PageSeparatorTable, PageSeparatorIndex),
      PageSeparatorPosition, PageSeparatorFontSize, RendererOutlineMode::Full,
      DefaultColorIndex);
  TipViewItems.Add(PageSeparator);
  // Current tip page
  CurrentPage = new Label();
  CurrentPage->Bounds = CurrentPageBounds;
  TipViewItems.Add(CurrentPage);
  // Total tip pages
  TotalPages = new Label();
  TotalPages->Bounds = TotalPagesBounds;
  TipViewItems.Add(TotalPages);
}

void TipsMenu::SwitchToTipId(int id) {
  CurrentlyDisplayedTipId = id;
  CurrentTipPage = 1;
  TipsSystem::SetTipUnreadState(id, false);
  TipsSystem::SetTipNewState(id, false);
  auto tipsScriptBufferId = TipsSystem::GetTipsScriptBufferId();

  auto tipRecord = TipsSystem::GetTipRecord(id);
  Name->SetText({.ScriptBufferId = tipsScriptBufferId,
                 .IpOffset = tipRecord->StringAdr[0]},
                NameFontSize, RendererOutlineMode::Full, DefaultColorIndex);
  Pronounciation->SetText({.ScriptBufferId = tipsScriptBufferId,
                           .IpOffset = tipRecord->StringAdr[1]},
                          PronounciationFontSize, RendererOutlineMode::Full,
                          DefaultColorIndex);

  Vm::Sc3VmThread dummy;
  dummy.IpOffset = tipRecord->StringAdr[2];
  dummy.ScriptBufferId = tipsScriptBufferId;
  float categoryWidth = TextGetPlainLineWidth(
      &dummy, Profile::Dialogue::DialogueFont, CategoryFontSize);
  Category->Bounds.X = CategoryEndX - categoryWidth;
  Category->SetText({.ScriptBufferId = tipsScriptBufferId,
                     .IpOffset = tipRecord->StringAdr[2]},
                    CategoryFontSize, RendererOutlineMode::Full,
                    DefaultColorIndex);
  if (tipRecord->ThumbnailIndex != 0xFFFF)
    ThumbnailSprite = &TipThumbnails[tipRecord->ThumbnailIndex];
  else
    ThumbnailSprite = &TipTextOnlyThumbnail;

  Number->SetText(fmt::format("{:4d}", tipRecord->Id + 1), NumberFontSize,
                  RendererOutlineMode::Full, DefaultColorIndex);

  CurrentPage->SetText(fmt::to_string(CurrentTipPage), PageSeparatorFontSize,
                       RendererOutlineMode::Full, DefaultColorIndex);

  TotalPages->SetText(fmt::to_string(tipRecord->NumberOfContentStrings),
                      PageSeparatorFontSize, RendererOutlineMode::Full,
                      DefaultColorIndex);

  TextPage.Clear();
  dummy.IpOffset = tipRecord->StringAdr[3];
  dummy.ScriptBufferId = tipsScriptBufferId;
  TextPage.AddString(&dummy);
}

void TipsMenu::NextTipPage() {
  CurrentTipPage += 1;
  auto currentRecord = TipsSystem::GetTipRecord(CurrentlyDisplayedTipId);
  if (CurrentTipPage > currentRecord->NumberOfContentStrings)
    CurrentTipPage = 1;

  TextPage.Clear();
  Vm::Sc3VmThread dummy;
  dummy.IpOffset = currentRecord->StringAdr[2 + CurrentTipPage];
  dummy.ScriptBufferId = TipsSystem::GetTipsScriptBufferId();
  TextPage.AddString(&dummy);
  CurrentPage->SetText(fmt::to_string(CurrentTipPage), PageSeparatorFontSize,
                       RendererOutlineMode::Full, DefaultColorIndex);
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto