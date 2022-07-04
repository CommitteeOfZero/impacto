#pragma once

#include "../button.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class SaveEntryButton : public Widgets::Button {
 public:
  SaveEntryButton(int id, Sprite const& norm, Sprite const& focused,
                  Sprite const& highlight, glm::vec2 pos);
  void Render() override;
  void AddEntryNumberHintText(uint8_t* str, int fontSize, bool outline);
  void AddPlayTimeHintText(uint8_t* str, int fontSize, bool outline);
  void AddPlayTimeText(std::string str, int fontSize, bool outline);
  void AddSaveDateHintText(uint8_t* str, int fontSize, bool outline);
  void AddSaveDateText(std::string str, int fontSize, bool outline);
  void AddSceneTitleText(uint8_t* str, int fontSize, bool outline);
  void AddThumbnail(Sprite thumbnail, glm::vec2 relativePosition);
  void Move(glm::vec2 pos);

  static void FocusedAlphaFadeStart();
  static void FocusedAlphaFadeReset();
  static void UpdateFocusedAlphaFade(float dt);

  bool EntryActive = false;

 private:
  Label NormalSpriteLabel;
  Label FocusedSpriteLabel;
  static glm::vec4 FocusedAlpha;
  static Animation FocusedAlphaFade;
  Label ThumbnailLabel;
  Label EntryNumberHint;
  Label SceneTitle;
  Label PlayTimeHint;
  Label PlayTime;
  Label SaveDateHint;
  Label SaveDate;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto