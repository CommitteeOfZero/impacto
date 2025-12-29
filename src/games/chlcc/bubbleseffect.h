#pragma once

#include <cstdint>
#include "../../animation.h"
namespace Impacto {
namespace UI {
namespace CHLCC {

struct Bubble {
  glm::vec2 Position{};
  uint16_t RandAngle{};
  uint16_t Size{};

  Bubble();
  void Init(bool loop = false);
  void Update(float dt);
  void Render(float alpha);
};

class BubblesEffect {
 public:
  BubblesEffect();
  void Init();
  void Update(float dt);
  void Render();

  static BubblesEffect& GetInstance() {
    static BubblesEffect impl;
    return impl;
  };

 private:
  Animation FadeAnimation;
  std::array<Bubble, 400> Bubbles;
};
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto