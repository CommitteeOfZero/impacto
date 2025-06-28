#pragma once

#include <string>

#include "../button.h"
#include "../label.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class SaveEntryButton : public Widgets::Button {
 public:
  SaveEntryButton(int id, Sprite const& norm, Sprite const& focused,
                  Sprite const& highlight, glm::vec2 pos, int page,
                  uint8_t locked, Sprite lockedSymbol);
  void Render() override;
  int GetPage() const;
  void AddNormalSpriteLabel(Sprite norm, glm::vec2 pos);
  void AddEntryNumberHintText(Vm::BufferOffsetContext strAdr, float fontSize,
                              RendererOutlineMode outlineMode,
                              glm::vec2 relativePosition);
  void AddEntryNumberText(std::string_view str, float fontSize,
                          RendererOutlineMode outlineMode,
                          glm::vec2 relativePosition);
  void AddPlayTimeHintText(Vm::BufferOffsetContext strAdr, float fontSize,
                           RendererOutlineMode outlineMode,
                           glm::vec2 relativePosition);
  void AddPlayTimeText(std::string_view str, float fontSize,
                       RendererOutlineMode outlineMode,
                       glm::vec2 relativePosition);
  void AddSaveDateHintText(Vm::BufferOffsetContext strAdr, float fontSize,
                           RendererOutlineMode outlineMode,
                           glm::vec2 relativePosition);
  void AddSaveDateText(std::string_view str, float fontSize,
                       RendererOutlineMode outlineMode,
                       glm::vec2 relativePosition);
  void AddSceneTitleText(Vm::BufferOffsetContext strAdr, float fontSize,
                         RendererOutlineMode outlineMode,
                         glm::vec2 relativeTitlePosition,
                         glm::vec2 relativeNoDataPosition);
  void AddThumbnail(Sprite thumbnail, glm::vec2 pos);
  void Move(glm::vec2 pos) override;

  static void FocusedAlphaFadeStart();
  static void FocusedAlphaFadeReset();
  static void UpdateFocusedAlphaFade(float dt);

  bool EntryActive = false;

 private:
  int Page;
  Label NormalSpriteLabel;
  Label FocusedSpriteLabel;
  Label LockedSymbol;
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

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto