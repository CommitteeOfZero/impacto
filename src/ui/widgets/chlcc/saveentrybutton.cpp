#include "saveentrybutton.h"

#include "../../../renderer2d.h"
#include "../../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

glm::vec4 SaveEntryButton::FocusedAlpha = glm::vec4(1.0f);
Animation SaveEntryButton::FocusedAlphaFade;

SaveEntryButton::SaveEntryButton(int id, Sprite const& norm,
                                 Sprite const& focused, Sprite const& highlight,
                                 glm::vec2 pos)
    : Widgets::Button(id, norm, focused, highlight, pos),
      NormalSpriteLabel(norm, pos),
      FocusedSpriteLabel(focused, pos) {}

void SaveEntryButton::Render() {
  NormalSpriteLabel.Render();
  if (HasFocus) {
    FocusedSpriteLabel.Tint = FocusedAlpha;
    FocusedSpriteLabel.Render();
  }
  ThumbnailLabel.Render();
  EntryNumberHint.Render();
  if (EntryActive) {
    SceneTitle.Render();
    PlayTimeHint.Render();
    PlayTime.Render();
    SaveDateHint.Render();
    SaveDate.Render();
  } else {
    SceneTitle.Render();
  }
}

void SaveEntryButton::AddEntryNumberHintText(uint8_t* str, int fontSize,
                                             bool outline) {
  EntryNumberHint = Label(str, glm::vec2(Bounds.X + 210.0f, Bounds.Y + 11),
                          fontSize, outline, 0);
}

void SaveEntryButton::AddSceneTitleText(uint8_t* str, int fontSize,
                                        bool outline) {
  if (EntryActive) {
    SceneTitle = Label(str, glm::vec2(Bounds.X + 210.0f, Bounds.Y + 36.0f),
                       fontSize, outline, 0);
  } else {
    SceneTitle = Label(str, glm::vec2(Bounds.X + 295.0f, Bounds.Y + 46.0f),
                       fontSize, outline, 0);
  }
}

void SaveEntryButton::AddPlayTimeHintText(uint8_t* str, int fontSize,
                                          bool outline) {
  PlayTimeHint = Label(str, glm::vec2(Bounds.X + 260.0f, Bounds.Y + 68.0f),
                       fontSize, outline, 0);
}

void SaveEntryButton::AddPlayTimeText(std::string str, int fontSize,
                                      bool outline) {
  // Spacing is currently set for the C;HLCC font, more or less
  PlayTime = Label(str, glm::vec2(Bounds.X + 420.0f, Bounds.Y + 68.0f),
                   fontSize, outline, 0);
}

void SaveEntryButton::AddSaveDateHintText(uint8_t* str, int fontSize,
                                          bool outline) {
  SaveDateHint = Label(str, glm::vec2(Bounds.X + 260.0f, Bounds.Y + 85.0f),
                       fontSize, outline, 0);
}

void SaveEntryButton::AddSaveDateText(std::string str, int fontSize,
                                      bool outline) {
  // Spacing is currently set for the C;HLCC font, more or less
  SaveDate = Label(str, glm::vec2(Bounds.X + 340.0f, Bounds.Y + 85.0f),
                   fontSize, outline, 0);
}

void SaveEntryButton::AddThumbnail(Sprite thumbnail,
                                   glm::vec2 relativePosition) {
  ThumbnailLabel =
      Label(thumbnail, glm::vec2(Bounds.X, Bounds.Y) + relativePosition);
}

void SaveEntryButton::Move(glm::vec2 relativePosition) {
  NormalSpriteLabel.Move(relativePosition);
  FocusedSpriteLabel.Move(relativePosition);
  ThumbnailLabel.Move(relativePosition);
  EntryNumberHint.Move(relativePosition);
  SceneTitle.Move(relativePosition);
  PlayTimeHint.Move(relativePosition);
  PlayTime.Move(relativePosition);
  SaveDateHint.Move(relativePosition);
  SaveDate.Move(relativePosition);
}

void SaveEntryButton::FocusedAlphaFadeStart() {
  if (FocusedAlphaFade.State == AS_Stopped) {
    FocusedAlphaFade.Direction = 1;
    FocusedAlphaFade.DurationIn = 0.5f;
    FocusedAlphaFade.DurationOut = 0.5f;
    FocusedAlphaFade.LoopMode = ALM_ReverseDirection;
    FocusedAlphaFade.StartIn();
  }
}

void SaveEntryButton::FocusedAlphaFadeReset() {
  FocusedAlphaFade.State = AS_Stopped;
  FocusedAlphaFade.Progress = 0.0f;
}

void SaveEntryButton::UpdateFocusedAlphaFade(float dt) {
  FocusedAlphaFade.Update(dt);
  FocusedAlpha =
      glm::vec4(glm::vec3(1.0f), ((FocusedAlphaFade.Progress * 30) + 1) / 85);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto