#include "tipsmenu.h"

#include "../../profile/ui/tipsmenu.h"
#include "../../profile/games/mo6tw/tipsmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../vm/interface/input.h"
#include "../../profile/scriptvars.h"
#include "../../profile/scriptinput.h"
#include "../../profile/dialogue.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/label.h"
#include "../../io/memorystream.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::TipsMenu;
using namespace Impacto::Profile::MO6TW::TipsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

TipsMenu::TipsMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  Name = new Widgets::Label();
  Name->Bounds = RectF(700.0f, 115.0f, 0.0f, 0.0f);
  Pronounciation = new Widgets::Label();
  Pronounciation->Bounds = RectF(700.0f, 85.0f, 0.0f, 0.0f);
  Category = new Widgets::Label();
  Category->Bounds = RectF(1135.0f, 205.0f, 0.0f, 0.0f);
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
    if (PADinputButtonWentDown & PAD1DOWN)
      SwitchToTipId(CurrentlyDisplayedTipId + 1);
    if (PADinputButtonWentDown & PAD1UP)
      SwitchToTipId(CurrentlyDisplayedTipId - 1);
  }
}

void TipsMenu::Render() {
  if (State != Hidden && ScrWork[SW_TIPSALPHA] > 0) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f), col);
    if (CurrentlyDisplayedTipId != -1) {
      Name->Render();
      Pronounciation->Render();
      Category->Render();
      Renderer2D::DrawProcessedText(TextPage->Glyphs, TextPage->Length,
                                    Profile::Dialogue::DialogueFont, col.a,
                                    true, true);
      if (ThumbnailSprite) {
        Renderer2D::DrawSprite(*ThumbnailSprite, glm::vec2(412.0f, 45.0f), col);
      }
    }
  }
}

void TipsMenu::DataInit(int scriptBufferId, uint8_t *tipsData) {
  auto scriptBuffer = Vm::ScriptBuffers[scriptBufferId];

  Impacto::Io::MemoryStream *stream =
      new Impacto::Io::MemoryStream(tipsData, MaxTipDataSize);
  auto unk01 = Impacto::Io::ReadLE<uint16_t>(stream);
  while (unk01 != 255) {
    TipsDataRecord record;
    memset(&record, 0, sizeof(TipsDataRecord));
    record.unk01 = unk01;
    record.thumbnailIndex = Impacto::Io::ReadLE<uint16_t>(stream);
    record.numberOfContentStrings = Impacto::Io::ReadLE<uint16_t>(stream);
    for (int i = 0; i < record.numberOfContentStrings + 3; i++) {
      record.stringPtrs[i] = Vm::ScriptGetStrAddress(
          scriptBuffer, Impacto::Io::ReadLE<uint16_t>(stream));
    }
    Records.push_back(record);
    unk01 = Impacto::Io::ReadLE<uint16_t>(stream);
  }
  delete stream;
}

void TipsMenu::SwitchToTipId(int id) {
  CurrentlyDisplayedTipId = id;
  auto tipRecord = Records.at(id);
  Name->SetText(tipRecord.stringPtrs[0], 34, true, 0);
  Pronounciation->SetText(tipRecord.stringPtrs[1], 22, true, 0);
  Category->SetText(tipRecord.stringPtrs[2], 34, true, 0);
  if (tipRecord.thumbnailIndex != 0xFFFF)
    ThumbnailSprite = &TipThumbnails[tipRecord.thumbnailIndex];
  else
    ThumbnailSprite = &TipTextOnlyThumbnail;

  TextPage->Clear();
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = tipRecord.stringPtrs[3];
  TextPage->AddString(&dummy);
}

void TipsMenu::SwitchToTipPage(int page) {}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto