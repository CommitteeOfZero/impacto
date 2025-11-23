#pragma once

#include "../../spriteanimation.h"
#include "../../ui/menu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
class DelusionTextSystem {
 public:
  static int constexpr MaxCharsPerLine = 90;
  std::array<std::array<const Sprite*, MaxCharsPerLine>, 3> GlyphLines;
  std::array<size_t, 300> LineOffsets{};

  std::optional<int> DelusionSelectedLine;
  size_t TextIndex = 0;
  float TextLineXOffset = 0;
  Animation DelusionTextFade;

  DelusionTextSystem();

  void Init();
  void Update(float dt);
  void Render();
  void Clear();

 private:
  void InitLineOffsets();
  void InitLines();
  void ScrollLine(size_t lineIndex);
  void FillLine(size_t& counter, std::span<const Sprite*> line);
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
  void UpdateShowing(float dt);
  void UpdateShown(float dt);
  void UpdateHiding(float dt);
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

  DelusionTextSystem TextSystem;

  friend DelusionTextSystem;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto