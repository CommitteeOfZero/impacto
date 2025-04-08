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
    Renderer->DrawProcessedText(SceneTitle, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::Full, true);
    Renderer->DrawProcessedText(PlayTimeHint, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::Full, true);
    Renderer->DrawProcessedText(PlayTime, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::Full, true);
    Renderer->DrawProcessedText(SaveDateHint, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::Full, true);
    Renderer->DrawProcessedText(SaveDate, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::Full, true);
  } else {
    Renderer->DrawProcessedText(SceneTitle, Profile::Dialogue::DialogueFont,
                                Tint.a, RendererOutlineMode::Full, true);
  }

  Renderer->DrawSprite(Thumbnail, glm::vec2(Bounds.X + 41.0f, Bounds.Y + 5.0f),
                       Tint);
}

void SaveEntryButton::AddSceneTitleText(uint8_t* str, float fontSize,
                                        bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  if (EntryActive) {
    SceneTitle = TextLayoutPlainLine(
        &dummy, 255, Profile::Dialogue::DialogueFont, fontSize,
        Profile::Dialogue::ColorTable[0], 1.0f,
        glm::vec2(Bounds.X + 228.0f, Bounds.Y + 10.0f), TextAlignment::Left);
  } else {
    SceneTitle = TextLayoutPlainLine(
        &dummy, 255, Profile::Dialogue::DialogueFont, fontSize,
        Profile::Dialogue::ColorTable[0], 1.0f,
        glm::vec2(Bounds.X + 228.0f, Bounds.Y + 50.0f), TextAlignment::Left);
  }
}

void SaveEntryButton::AddPlayTimeHintText(uint8_t* str, float fontSize,
                                          bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  PlayTimeHint = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 238.0f, Bounds.Y + 45.0f), TextAlignment::Left);
}

void SaveEntryButton::AddPlayTimeText(uint8_t* str, float fontSize,
                                      bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  PlayTime = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 372.0f, Bounds.Y + 61.0f), TextAlignment::Left);
}

void SaveEntryButton::AddSaveDateHintText(uint8_t* str, float fontSize,
                                          bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  SaveDateHint = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 238.0f, Bounds.Y + 75.0f), TextAlignment::Left);
}

void SaveEntryButton::AddSaveDateText(uint8_t* str, float fontSize,
                                      bool outline) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  SaveDate = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[0], 1.0f,
      glm::vec2(Bounds.X + 283.0f, Bounds.Y + 93.0f), TextAlignment::Left);
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto