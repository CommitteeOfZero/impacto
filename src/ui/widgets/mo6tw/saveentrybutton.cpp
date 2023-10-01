#include "saveentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

void SaveEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else {
    if (Enabled) {
      Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    } else {
      Renderer->DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    }
  }

  if (EntryActive) {
    Renderer->DrawProcessedText(SceneTitle, SceneTitleLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RO_Full, true);
    Renderer->DrawProcessedText(PlayTimeHint, PlayTimeHintLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RO_Full, true);
    Renderer->DrawProcessedText(PlayTime, PlayTimeLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RO_Full, true);
    Renderer->DrawProcessedText(SaveDateHint, SaveDateHintLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RO_Full, true);
    Renderer->DrawProcessedText(SaveDate, SaveDateLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RO_Full, true);
  } else {
    Renderer->DrawProcessedText(SceneTitle, SceneTitleLength,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                RO_Full, true);
  }

  Renderer->DrawSprite(Thumbnail, glm::vec2(Bounds.X + 41.0f, Bounds.Y + 5.0f),
                       Tint);
}

void SaveEntryButton::AddSceneTitleText(uint8_t* str, int fontSize,
                                        bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  if (EntryActive) {
    SceneTitleLength = TextLayoutPlainLine(
        &dummy, 255, SceneTitle, Profile::Dialogue::DialogueFont, fontSize,
        Profile::Dialogue::ColorTable[0], 1.0f,
        glm::vec2(Bounds.X + 228.0f, Bounds.Y + 10.0f), TextAlignment::Left);
  } else {
    SceneTitleLength = TextLayoutPlainLine(
        &dummy, 255, SceneTitle, Profile::Dialogue::DialogueFont, fontSize,
        Profile::Dialogue::ColorTable[0], 1.0f,
        glm::vec2(Bounds.X + 228.0f, Bounds.Y + 50.0f), TextAlignment::Left);
  }
}

void SaveEntryButton::AddPlayTimeHintText(uint8_t* str, int fontSize,
                                          bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  PlayTimeHintLength = TextLayoutPlainLine(
      &dummy, 255, PlayTimeHint, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 238.0f, Bounds.Y + 45.0f), TextAlignment::Left);
}

void SaveEntryButton::AddPlayTimeText(uint8_t* str, int fontSize,
                                      bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  PlayTimeLength = TextLayoutPlainLine(
      &dummy, 255, PlayTime, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 372.0f, Bounds.Y + 61.0f), TextAlignment::Left);
}

void SaveEntryButton::AddSaveDateHintText(uint8_t* str, int fontSize,
                                          bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  SaveDateHintLength = TextLayoutPlainLine(
      &dummy, 255, SaveDateHint, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 238.0f, Bounds.Y + 75.0f), TextAlignment::Left);
}

void SaveEntryButton::AddSaveDateText(uint8_t* str, int fontSize,
                                      bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  SaveDateLength = TextLayoutPlainLine(
      &dummy, 255, SaveDate, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 283.0f, Bounds.Y + 93.0f), TextAlignment::Left);
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto