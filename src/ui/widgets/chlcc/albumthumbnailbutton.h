#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class AlbumThumbnailButton : public Button {
 public:
  AlbumThumbnailButton(int id, Sprite const& norm, Sprite const& focused,
                       Sprite const& highlight, glm::vec2 pos,
                       int totalVariations, int viewedVariations,
                       Sprite const& unlockedVariation,
                       Sprite const& lockedVariation, glm::vec2 variationOffset,
                       Sprite locked, Sprite SelectionMarker,
                       glm::vec2 SelectionMarkerOffset)
      : Button(id, norm, focused, highlight, pos),
        TotalVariations(totalVariations),
        ViewedVariations(viewedVariations),
        VariationUnlocked(unlockedVariation),
        VariationLocked(lockedVariation),
        VariationTemplateOffset(variationOffset),
        SelectionMarker(SelectionMarker),
        SelectionMarkerOffset(SelectionMarkerOffset) {
    IsLocked = (viewedVariations == 0);
    LockedSprite = locked;
  }

  void Render() override;

  void UpdateVariations(int total, int viewed);
  static void FocusedAlphaFadeStart();
  static void FocusedAlphaFadeReset();
  static void UpdateFocusedAlphaFade(float dt);

 private:
  int TotalVariations;
  int ViewedVariations;
  Sprite VariationUnlocked;
  Sprite VariationLocked;
  glm::vec2 VariationTemplateOffset;
  Sprite SelectionMarker;
  glm::vec2 SelectionMarkerOffset;

  static glm::vec4 FocusedAlpha;
  static Animation FocusedAlphaFade;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto