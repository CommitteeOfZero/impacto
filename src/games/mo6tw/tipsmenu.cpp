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
#include "../../io/memorystream.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::TipsMenu;
using namespace Impacto::Profile::MO6TW::TipsMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

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

  Records.clear();
  Records.shrink_to_fit();

  ItemsList = new Widgets::Carousel(CDIR_HORIZONTAL);
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto onClick = std::bind(&TipsMenu::TipOnClick, this, std::placeholders::_1);
  int idx = 0, currentPage = 0, idxInPage = 0;
  Widgets::Group *pageItems = new Widgets::Group(this);

  Io::MemoryStream *stream = new Io::MemoryStream(tipsData, MaxTipDataSize);
  auto unk01 = Io::ReadLE<uint16_t>(stream);
  while (unk01 != 255) {
    TipsDataRecord record;
    memset(&record, 0, sizeof(TipsDataRecord));
    record.id = idx++;
    // I don't know, I don't care, this is not my magic
    record.sortLetterIndex = (unk01 - 5 * ((unk01 + 1) / 10) - 6);
    auto page = record.sortLetterIndex / 5;
    if (page != currentPage) {
      currentPage = page;
      idxInPage = 0;
      pageItems->Show();
      ItemsList->Add(pageItems);
      pageItems = new Widgets::Group(this);
    }
    record.thumbnailIndex = Io::ReadLE<uint16_t>(stream);
    record.numberOfContentStrings = Io::ReadLE<uint16_t>(stream);
    for (int i = 0; i < record.numberOfContentStrings + 3; i++) {
      record.stringPtrs[i] =
          Vm::ScriptGetStrAddress(scriptBuffer, Io::ReadLE<uint16_t>(stream));
    }
    Records.push_back(record);

    Button *button = new Button(
        record.id, nullSprite, nullSprite, Profile::BacklogMenu::EntryHighlight,
        glm::vec2(119.0f, 108.0f + (idxInPage++ * 20.0f)));

    button->HighlightOffset.y = -10.0f;
    button->OnClickHandler = onClick;
    button->SetText(record.stringPtrs[0], 20, true, 0);
    pageItems->Add(button, FDIR_DOWN);

    unk01 = Io::ReadLE<uint16_t>(stream);
  }

  delete stream;
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