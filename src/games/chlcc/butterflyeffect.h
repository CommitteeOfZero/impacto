#pragma once

// Hey this isn't Steins;Gate

#include <cstdint>
#include "../../animation.h"
namespace Impacto {
namespace UI {
namespace CHLCC {

struct Butterfly {
  glm::vec2 Position{};
  glm::vec2 Velocity{};
  Animation Flap;
  uint16_t Size{};

  Butterfly();
  void Init(bool loop = false);
  void Update(float dt);
  void Render(float alpha);
};

class ButterflyEffect {
 public:
  ButterflyEffect();
  void Init();
  void Update(float dt);
  void Render();

  static ButterflyEffect& GetInstance() {
    static ButterflyEffect impl;
    return impl;
  };

 private:
  Animation FadeAnimation;
  std::array<Butterfly, 400> Butterflies;
};
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto