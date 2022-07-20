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
                  Sprite const& highlight, glm::vec2 pos, int page);
  void Render() override;
  int GetPage() const;
  void AddNormalSpriteLabel(Sprite norm, glm::vec2 pos);
  void AddEntryNumberHintText(uint8_t* str, int fontSize, bool outline,
                              glm::vec2 pos);
  void AddEntryNumberText(std::string str, int fontSize, bool outline,
                          glm::vec2 pos);
  void AddPlayTimeHintText(uint8_t* str, int fontSize, bool outline,
                           glm::vec2 pos);
  void AddPlayTimeText(std::string str, int fontSize, bool outline,
                       glm::vec2 pos);
  void AddSaveDateHintText(uint8_t* str, int fontSize, bool outline,
                           glm::vec2 pos);
  void AddSaveDateText(std::string str, int fontSize, bool outline,
                       glm::vec2 pos);
  void AddSceneTitleText(uint8_t* str, int fontSize, bool outline,
                         glm::vec2 pos);
  void AddThumbnail(Sprite thumbnail, glm::vec2 pos);
  void Move(glm::vec2 pos);

  static void FocusedAlphaFadeStart();
  static void FocusedAlphaFadeReset();
  static void UpdateFocusedAlphaFade(float dt);

  bool EntryActive = false;

 private:
  int Page;
  Label NormalSpriteLabel;
  Label FocusedSpriteLabel;
  static glm::vec4 FocusedAlpha;
  static Animation FocusedAlphaFade;
  Label ThumbnailLabel;
  Label EntryNumberHint;
  Label EntryNumber;
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