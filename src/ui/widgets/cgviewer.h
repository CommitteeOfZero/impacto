#pragma once

#include <functional>

#include "../widget.h"
#include "../menu.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../profile/data/savesystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {

int constexpr MaxCgViewerCgs = 3;
int constexpr MaxCgViewerVariations = 15;

class CgViewer : public Widget {
 public:
  CgViewer(std::optional<float> fadeDuration = std::nullopt);

  void Show() override;
  void Hide() override;

  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  void RenderVariation(size_t index, glm::vec4 col) const;

  void LoadCgSprites(size_t evId, std::string mountPoint,
                     uint16_t loadIds[][Profile::SaveSystem::MaxCGSprites]);

  bool IsOnLastVariation() const;
  std::function<void(CgViewer*)> OnVariationEndHandler;

  bool IsFadingBetweenVariations() const {
    return State == Shown && this->FadeAnimation.IsPlaying() &&
           this->FadeAnimation.Direction == AnimationDirection::In;
  };

  float GetVariationFadeProgress() const {
    return this->FadeAnimation.Progress;
  }

  bool IsShown() const { return State == Shown; };
  bool IsHidden() const { return State == Hidden; };

 protected:
  Texture CgTexture;
  Sprite CgSprites[MaxCgViewerVariations][MaxCgViewerCgs];
  SpriteSheet CgSpriteSheets[MaxCgViewerVariations][MaxCgViewerCgs];

  size_t EvId;

  std::array<int, MaxCgViewerVariations> CgCount;
  std::array<bool, MaxCgViewerVariations> HorizontalRendering;
  MenuState State = Hidden;

  glm::vec2 Position[MaxCgViewerVariations];
  float PrevScale = 1.0f;
  float Scale = 1.0f;
  float MinScale[MaxCgViewerVariations];
  size_t CurrentVariation = 0;
  int VariationCount = 0;

  float MouseDownTime = 0.0f;

  void Clear();

  Animation FadeAnimation;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto