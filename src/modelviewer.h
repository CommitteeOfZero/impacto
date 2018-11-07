#pragma once

#include "impacto.h"

#include <glm/glm.hpp>

namespace Impacto {

class Game;

class ModelViewer {
 public:
  ModelViewer(Game* game);
  ~ModelViewer();

  void Init();
  void Update(float dt);

 private:
  void EnumerateBgm();

  Game* GameContext;

  glm::vec3 CameraPosition;
  glm::vec3 CameraTarget;
  int TrackCamera;
  nk_colorf UiTintColor;
  uint32_t CurrentModel;
  uint32_t CurrentAnim;
  uint32_t CurrentBackground;
  uint32_t CurrentBgm;
  int UiWindowWidth;
  int UiWindowHeight;
  int UiMsaaCount;

  char** BgmNames = 0;
  uint32_t* BgmIds = 0;
  uint32_t BgmCount = 0;
  bool BgmChangeQueued;

  float BgmFadeOut;
  float BgmFadeIn;
  int BgmLoop;

  // FPS counter
  float LastTime;
  int Frames;
  float FPS;
};

}  // namespace Impacto