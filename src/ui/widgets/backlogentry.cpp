#include "backlogentry.h"
#include "../../renderer/renderer.h"
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

BacklogEntry::BacklogEntry(int id, uint8_t* str, int audioId, glm::vec2 pos)
    : Position(pos) {
  Enabled = true;
  Id = id;
  AudioId = audioId;

  BacklogPage = new DialoguePage();
  BacklogPage->Glyphs.reserve(512);
  BacklogPage->Clear();
  BacklogPage->Mode = DPM_REV;

  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  // CHLCC uses DPM_REV for the Erin DialogueBox
  if (Profile::Dialogue::DialogueBoxCurrentType != +DialogueBoxType::CHLCC) {
    Profile::Dialogue::REVBounds.X = pos.x;
    Profile::Dialogue::REVBounds.Y = pos.y;
  }
  BacklogPage->AddString(&dummy);

  TextLength = BacklogPage->Length;

  if (TextLength > 0) {
    Position = glm::vec2(BacklogPage->Glyphs[0].DestRect.X,
                         BacklogPage->Glyphs[0].DestRect.Y);
    for (int chr = 1; chr < TextLength; chr++) {
      Position.x = std::min(Position.x, BacklogPage->Glyphs[chr].DestRect.X);
      Position.y = std::min(Position.y, BacklogPage->Glyphs[chr].DestRect.Y);
    }
  }

  switch (BacklogPage->Alignment) {
    case TextAlignment::Left:
      break;
    case TextAlignment::Center: {
      const RectF& revBounds = Profile::Dialogue::REVBounds;
      pos.x = revBounds.X + (revBounds.Width - BacklogPage->Dimensions.x) / 2;
      break;
    }
  }
  MoveTo(pos);

  float currentY = BacklogPage->Glyphs[0].DestRect.Y;
  float currentWidth = 0.0f, maxWidth = 0.0f;
  for (int i = 0; i < BacklogPage->Glyphs.size(); i++) {
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
    TextHeight = (BacklogPage->Glyphs.back().DestRect.Y +
                  BacklogPage->Glyphs.back().DestRect.Height) -
                 BacklogPage->Name[0].DestRect.Y;
    Bounds = RectF(BacklogPage->Name[0].DestRect.X,
                   BacklogPage->Name[0].DestRect.Y, maxWidth, TextHeight);
  } else {
    TextHeight = (BacklogPage->Glyphs.back().DestRect.Y +
                  BacklogPage->Glyphs.back().DestRect.Height) -
                 BacklogPage->Glyphs[0].DestRect.Y;
    Bounds = RectF(BacklogPage->Glyphs[0].DestRect.X,
                   BacklogPage->Glyphs[0].DestRect.Y, maxWidth, TextHeight);
  }
}

BacklogEntry::~BacklogEntry() { delete BacklogPage; }

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
  Position += relativePosition;
  Widget::Move(relativePosition);
  ProcessedTextGlyph::Move(BacklogPage->Glyphs, TextLength, relativePosition);
  if (BacklogPage->HasName)
    ProcessedTextGlyph::Move(BacklogPage->Name, BacklogPage->NameLength,
                             relativePosition);
}

void BacklogEntry::MoveTo(glm::vec2 position) {
  glm::vec2 relativePosition = position - Position;
  Move(relativePosition);
}

void BacklogEntry::Render() {
  if (AudioId != -1) {
    Renderer->DrawSprite(
        VoiceIcon, glm::vec2(Bounds.X - VoiceIcon.ScaledWidth(), Bounds.Y));
  }

  if (BacklogPage->HasName) {
    Renderer->DrawProcessedText(BacklogPage->Name,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                Profile::Dialogue::REVOutlineMode, true);
  }

  Renderer->DrawProcessedText(BacklogPage->Glyphs,
                              Profile::Dialogue::DialogueFont, Tint.a,
                              Profile::Dialogue::REVOutlineMode, true);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto