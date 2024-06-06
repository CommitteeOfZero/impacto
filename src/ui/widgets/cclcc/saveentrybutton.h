#pragma once

#include <string>

#include "../button.h"
#include "../label.h"
#include "../../../renderer/renderer.h"
#include "../../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class SaveEntryButton : public Widgets::Button {
 public:
  SaveEntryButton(int id, Sprite const& focusedBox, Sprite const& focusedText,
                  int page, glm::vec2 pos, uint8_t locked, Sprite lockedSymbol,
                  SaveSystem::SaveType saveType, Sprite NoDataSprite,
                  Sprite BrokenDataSprite);
  void Render() override;
  int GetPage() const;
  void AddNormalSpriteLabel(Sprite norm, glm::vec2 pos);
  void AddSaveDateText(std::string str, float fontSize,
                       RendererOutlineMode outlineMode,
                       glm::vec2 relativePosition);
  void RenderSceneTitleText();
  void RenderSaveDateText();
  void AddThumbnail(Sprite thumbnail, glm::vec2 pos);
  void Move(glm::vec2 pos) override;

  static void FocusedAlphaFadeStart();
  static void FocusedAlphaFadeReset();
  static void UpdateFocusedAlphaFade(float dt);

 private:
  int Page;
  Label NormalSpriteLabel;
  Label FocusedSpriteLabel;
  Label LockedSymbol;
  static glm::vec4 FocusedAlpha;
  static Animation FocusedAlphaFade;
  Label ThumbnailLabel;
  SaveSystem::SaveType Type;
  Label NoDataSymbol;
  Label BrokenDataSymbol;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto