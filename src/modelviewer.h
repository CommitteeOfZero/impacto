#pragma once

#include "impacto.h"

#include <glm/glm.hpp>

namespace Impacto {

class ModelViewer {
 public:
  ModelViewer();

  void Init();
  void Update(float dt);

 private:
  glm::vec3 CameraPosition;
  glm::vec3 CameraTarget;
  nk_colorf UiTintColor;
  uint32_t CurrentModel;
  uint32_t CurrentAnim;
  uint32_t CurrentBackground;
  int UiWindowWidth;
  int UiWindowHeight;
  int UiMsaaCount;
};

}  // namespace Impacto