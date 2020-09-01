#include "saveentrybutton.h"

#include "../../../renderer2d.h"
#include "../../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

void SaveEntryButton::Render() {
  glm::vec4 white(1.0f);
  white.a = Opacity;

  if (HasFocus) {
    Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), white);
  } else {
    if (Enabled) {
      Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y),
                             white);
    } else {
      Renderer2D::DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y),
                             white);
    }
  }

  if (EntryActive) {
    Renderer2D::DrawProcessedText(SceneTitle, SceneTitleLength,
                                  Profile::Dialogue::DialogueFont, Opacity,
                                  true, true);
    Renderer2D::DrawProcessedText(PlayTimeHint, PlayTimeHintLength,
                                  Profile::Dialogue::DialogueFont, Opacity,
                                  true, true);
    Renderer2D::DrawProcessedText(PlayTime, PlayTimeLength,
                                  Profile::Dialogue::DialogueFont, Opacity,
                                  true, true);
    Renderer2D::DrawProcessedText(SaveDateHint, SaveDateHintLength,
                                  Profile::Dialogue::DialogueFont, Opacity,
                                  true, true);
    Renderer2D::DrawProcessedText(SaveDate, SaveDateLength,
                                  Profile::Dialogue::DialogueFont, Opacity,
                                  true, true);
  } else {
    Renderer2D::DrawProcessedText(SceneTitle, SceneTitleLength,
                                  Profile::Dialogue::DialogueFont, Opacity,
                                  true, true);
  }

  Renderer2D::DrawSprite(Thumbnail,
                         glm::vec2(Bounds.X + 41.0f, Bounds.Y + 5.0f), white);
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