#pragma once

#include "basegame.h"

namespace Impacto {

class ModelViewerGame : public BaseGame {
 public:
  ModelViewerGame();

 private:
  void GameUpdate(float dt) override;
  void DrawLayer(uint32_t layerId) override;

  glm::vec3 CameraPosition;
  glm::vec3 CameraTarget;
  nk_colorf UiTintColor;
  uint32_t CurrentModel;
  uint32_t CurrentAnim;
  uint32_t CurrentBackground;
  int UiWindowWidth;
  int UiWindowHeight;
  bool UiWindowDimsNeedUpdate;
};

}  // namespace Impacto