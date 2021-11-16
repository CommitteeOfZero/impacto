#include "backlogentry.h"
#include "../../renderer2d.h"
#include "../../inputsystem.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"
#include "../../vm/thread.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::BacklogMenu;

BacklogEntry::BacklogEntry(int id, uint8_t* str, int audioId, glm::vec2 pos) {
  Enabled = true;
  Id = id;
  AudioId = audioId;

  BacklogPage = new DialoguePage();
  BacklogPage->Glyphs = new ProcessedTextGlyph[255];
  BacklogPage->Clear();
  BacklogPage->Mode = DPM_REV;

  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  Profile::Dialogue::REVBounds.X = pos.x;
  Profile::Dialogue::REVBounds.Y = pos.y;
  BacklogPage->AddString(&dummy);
  TextLength = BacklogPage->Length;
  float currentY = BacklogPage->Glyphs[0].DestRect.Y;
  float currentWidth = 0.0f, maxWidth = 0.0f;
  for (int i = 0; i < TextLength; i++) {
    BacklogPage->Glyphs[i].Opacity = 1.0f;
    if (currentY != BacklogPage->Glyphs[i].DestRect.Y) {
      if (currentWidth > maxWidth) {
        maxWidth = currentWidth;
      }
      currentWidth = 0.0f;
      currentY = BacklogPage->Glyphs[i].DestRect.Y;
    }
    currentWidth += BacklogPage->Glyphs[i].DestRect.Width;
  }
  if (currentWidth > maxWidth) {
    maxWidth = currentWidth;
  }
  if (BacklogPage->HasName) {
    TextHeight = (BacklogPage->Glyphs[TextLength - 1].DestRect.Y +
                  BacklogPage->Glyphs[TextLength - 1].DestRect.Height) -
                 BacklogPage->Name[0].DestRect.Y;
    Bounds = RectF(BacklogPage->Name[0].DestRect.X,
                   BacklogPage->Name[0].DestRect.Y, maxWidth, TextHeight);
  } else {
    TextHeight = (BacklogPage->Glyphs[TextLength - 1].DestRect.Y +
                  BacklogPage->Glyphs[TextLength - 1].DestRect.Height) -
                 BacklogPage->Glyphs[0].DestRect.Y;
    Bounds = RectF(BacklogPage->Glyphs[0].DestRect.X,
                   BacklogPage->Glyphs[0].DestRect.Y, maxWidth, TextHeight);
  }
}

BacklogEntry::~BacklogEntry() {
  delete BacklogPage->Glyphs;
  delete BacklogPage;
}

void BacklogEntry::UpdateInput() {
  if (Enabled) {
    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
    }
    if (HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) ||
         (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1A))) {
      OnClickHandler(this);
    }
  }
}

void BacklogEntry::Move(glm::vec2 relativePosition) {
  Widget::Move(relativePosition);
  for (int i = 0; i < TextLength; i++) {
    BacklogPage->Glyphs[i].DestRect.X += relativePosition.x;
    BacklogPage->Glyphs[i].DestRect.Y += relativePosition.y;
  }
  if (BacklogPage->HasName) {
    for (int i = 0; i < BacklogPage->NameLength; i++) {
      BacklogPage->Name[i].DestRect.X += relativePosition.x;
      BacklogPage->Name[i].DestRect.Y += relativePosition.y;
    }
  }
}

void BacklogEntry::Render() {
  if (HasFocus) {
    Renderer2D::DrawSprite(
        EntryHighlight,
        RectF(
            Bounds.X, Bounds.Y + Bounds.Height - EntryHighlight.ScaledHeight(),
            Profile::Dialogue::REVBounds.Width, EntryHighlight.ScaledHeight()));
  }
  if (AudioId != -1) {
    Renderer2D::DrawSprite(
        VoiceIcon, glm::vec2(Bounds.X - VoiceIcon.ScaledWidth(), Bounds.Y));
  }
  if (BacklogPage->HasName) {
    Renderer2D::DrawProcessedText(BacklogPage->Name, BacklogPage->NameLength,
                                  Profile::Dialogue::DialogueFont, Tint.a, true,
                                  true);
  }
  Renderer2D::DrawProcessedText(BacklogPage->Glyphs, TextLength,
                                Profile::Dialogue::DialogueFont, Tint.a, true,
                                true);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto