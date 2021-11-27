#include "tipsmenu.h"

#include "../../profile/ui/tipsmenu.h"
#include "../../profile/games/mo6tw/tipsmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/mo6tw/tipsentrybutton.h"
#include "../../io/memorystream.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::TipsMenu;
using namespace Impacto::Profile::MO6TW::TipsMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::MO6TW;

void TipsMenu::TipOnClick(Widgets::Button *target) {
  SwitchToTipId(target->Id);
}

TipsMenu::TipsMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  Name = new Widgets::Label();
  Name->Bounds = RectF(703.0f, 117.0f, 0.0f, 0.0f);
  Pronounciation = new Widgets::Label();
  Pronounciation->Bounds = RectF(703.0f, 87.0f, 0.0f, 0.0f);
  Category = new Widgets::Label();
  Category->Bounds = RectF(1135.0f, 206.0f, 0.0f, 0.0f);
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

  if (State == Shown) {
    ItemsList->UpdateInput();
    ItemsList->Update(dt);
  }
}

void TipsMenu::Render() {
  if (State != Hidden && ScrWork[SW_TIPSALPHA] > 0) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f), col);
    ItemsList->Render();

    if (CurrentlyDisplayedTipId != -1) {
      NumberText->Render();
      Number->Render();
      PageSeparator->Render();
      CurrentPage->Render();
      TotalPages->Render();
      Name->Render();
      Pronounciation->Render();
      Category->Render();
      Renderer2D::DrawProcessedText(TextPage->Glyphs, TextPage->Length,
                                    Profile::Dialogue::DialogueFont, col.a,
                                    true, true);
      if (ThumbnailSprite) {
        Renderer2D::DrawSprite(*ThumbnailSprite, glm::vec2(413.0f, 45.0f), col);
      }
    }
  }
}

void TipsMenu::DataInit(int scriptBufferId, uint8_t *tipsData) {
  auto scriptBuffer = Vm::ScriptBuffers[scriptBufferId];

  ItemsList = new Widgets::Carousel(CDIR_HORIZONTAL);
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto onClick = std::bind(&TipsMenu::TipOnClick, this, std::placeholders::_1);
  int idx = 0, currentPage = 0, currentCategoryId = -1;
  Widgets::Group *pageItems = new Widgets::Group(this);

  // String of characters by which tips are sorted, taken from _system script
  sortString = (uint16_t *)Vm::ScriptGetTextTableStrAddress(2, 5);

  float currentY = 87.0f;

  // Read tips data from the script and create UI elements for each tip
  Io::MemoryStream *stream = new Io::MemoryStream(tipsData, MaxTipDataSize);
  auto unk01 = Io::ReadLE<uint16_t>(stream);
  while (unk01 != 255) {
    // Read tip entry from the data array
    TipsDataRecord record;
    memset(&record, 0, sizeof(TipsDataRecord));
    record.id = idx++;
    // I don't know, I don't care, this is not my magic
    record.sortLetterIndex = (unk01 - 5 * ((unk01 + 1) / 10) - 6);
    record.thumbnailIndex = Io::ReadLE<uint16_t>(stream);
    record.numberOfContentStrings = Io::ReadLE<uint16_t>(stream);
    for (int i = 0; i < record.numberOfContentStrings + 3; i++) {
      record.stringPtrs[i] =
          Vm::ScriptGetStrAddress(scriptBuffer, Io::ReadLE<uint16_t>(stream));
    }
    Records.push_back(record);

    // In this case the tips are split into pages in UI with maximum of 5
    // categories per page, with each category being a character from
    // the sort string and containing all tips the names of which begin with
    // that character
    auto page = record.sortLetterIndex / 5;
    if (page != currentPage) {
      currentPage = page;
      currentY = 87.0f;
      pageItems->Show();
      ItemsList->Add(pageItems);
      pageItems = new Widgets::Group(this);
    }

    // Start new category
    // We take a character from the sort string and use that as the category
    // name inside a predefined template
    if (record.sortLetterIndex != currentCategoryId) {
      currentCategoryId = record.sortLetterIndex;
      CategoryString[1] = sortString[currentCategoryId];

      Label *categoryLabel = new Label();
      categoryLabel->Bounds.X = 69.0f;
      categoryLabel->Bounds.Y = currentY;
      categoryLabel->SetText((uint8_t *)CategoryString, 20, true, 0);
      pageItems->Add(categoryLabel);
      currentY += 20.0f;
    }

    // Actual tip entry button
    TipsEntryButton *button = new TipsEntryButton(
        record.id, &record, RectF(69.0f, currentY, 300.0f, 24.0f),
        Profile::BacklogMenu::EntryHighlight);
    button->OnClickHandler = onClick;

    pageItems->Add(button, FDIR_DOWN);
    currentY += 20.0f;

    // Next tip entry from the data array
    unk01 = Io::ReadLE<uint16_t>(stream);
  }

  delete stream;

  // Add last category
  pageItems->Show();
  ItemsList->Add(pageItems);

  // Number label
  NumberText = new Label(Vm::ScriptGetTextTableStrAddress(2, 8),
                         glm::vec2(1070.0f, 45.0f), 32, true, 0);
  // Tip number
  Number = new Label();
  Number->Bounds = RectF(1130.0f, 45.0f, 0.0f, 0.0f);
  // Tip page separator
  PageSeparator = new Label(Vm::ScriptGetTextTableStrAddress(2, 9),
                            glm::vec2(1154.0f, 645.0f), 32, true, 0);
  // Current tip page
  CurrentPage = new Label();
  CurrentPage->Bounds = RectF(1130.0f, 645.0f, 0.0f, 0.0f);
  // Total tip pages
  TotalPages = new Label();
  TotalPages->Bounds = RectF(1184.0f, 645.0f, 0.0f, 0.0f);
}

void TipsMenu::SwitchToTipId(int id) {
  CurrentlyDisplayedTipId = id;
  auto tipRecord = Records.at(id);
  Name->SetText(tipRecord.stringPtrs[0], 32, true, 0);
  Pronounciation->SetText(tipRecord.stringPtrs[1], 20, true, 0);

  Vm::Sc3VmThread dummy;
  dummy.Ip = tipRecord.stringPtrs[2];
  float categoryWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  Category->Bounds.X = 1200.0f - categoryWidth;
  Category->SetText(tipRecord.stringPtrs[2], 32, true, 0);
  if (tipRecord.thumbnailIndex != 0xFFFF)
    ThumbnailSprite = &TipThumbnails[tipRecord.thumbnailIndex];
  else
    ThumbnailSprite = &TipTextOnlyThumbnail;

  char temp[5];
  sprintf(temp, "%4d", tipRecord.id + 1);
  Number->SetText(std::string(temp), 32, true, 0);

  sprintf(temp, "%d", CurrentTipPage);
  CurrentPage->SetText(std::string(temp), 32, true, 0);

  sprintf(temp, "%d", tipRecord.numberOfContentStrings);
  TotalPages->SetText(std::string(temp), 32, true, 0);

  TextPage->Clear();
  dummy.Ip = tipRecord.stringPtrs[3];
  TextPage->AddString(&dummy);
}

void TipsMenu::SwitchToTipPage(int num) {
  CurrentTipPage = num;
  TextPage->Clear();
  Vm::Sc3VmThread dummy;
  dummy.Ip = Records.at(CurrentlyDisplayedTipId).stringPtrs[2 + num];
  TextPage->AddString(&dummy);
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto