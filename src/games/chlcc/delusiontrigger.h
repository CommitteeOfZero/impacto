#pragma once

#include "../../spriteanimation.h"
#include "../../ui/menu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
class DelusionTrigger;
class DelusionTextSystem {
 public:
  static int constexpr LineWidth = 60;
  std::array<std::array<const Sprite*, 60>, 3> GlyphLines;
  std::array<size_t, 300> LineOffsets{};

  size_t TextIndex = 0;
  int TextLineXOffset = 0;
  const DelusionTrigger* Ctx;

  DelusionTextSystem(DelusionTrigger const& ctx);

  void Init();
  void Update();
  void Render();
  void Clear();

 private:
  std::optional<size_t> LineBase() const;
  void InitLineOffsets();
  void InitLines();
  void ScrollLine(int lineIndex);
  void ScrollAll();
};

class DelusionTrigger {
 public:
  enum DelusionState { DS_Neutral, DS_Positive, DS_Negative };
  DelusionTrigger();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();
  void Load();
  void Reset();

  static DelusionTrigger& GetInstance() {
    static DelusionTrigger impl;
    return impl;
  };

  UI::MenuState State = UI::Hidden;

 protected:
  void UpdateShowing();
  void UpdateShown();
  void UpdateHiding();
  void PlayClickSound();

  int& DelusionState;

  int MaskScaleFactor;
  int SpinAngle;
  int SpinRate;
  int UnderlayerAlpha;
  int BackgroundAlpha;

  int AnimCounter;
  int AnimationState;

  glm::vec4 TriggerOnTint;
  int TriggerOnTintAlpha;

  int UnderlayerXOffset, UnderlayerXRate;
  int ShakeState;
  int MaskOffsetX;
  std::optional<int> DelusionSelectedLine;
  int DelusionTextAlpha;

  DelusionTextSystem TextSystem;

  friend DelusionTextSystem;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto