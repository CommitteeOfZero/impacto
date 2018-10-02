#pragma once

#include "3d/scene.h"
#include "impacto.h"
#include "modelviewer.h"

namespace Impacto {

enum GameFeature {
  GameFeature_Nuklear = (1 << 0),
  GameFeature_Scene3D = (1 << 1),
  GameFeature_ModelViewer = (1 << 2)
};

struct GameFeatureConfig {
  uint32_t LayerCount;
  uint32_t GameFeatures;

  uint32_t Scene3D_CharacterCount;
  uint32_t Scene3D_BackgroundCount;
};

class Game {
 public:
  static void InitModelViewer();

  ~Game();
  void Update(float dt);
  void Render();

  Scene* Scene3D = 0;
  nk_context* Nk = 0;
  ModelViewer* ModelViewerComponent = 0;

  uint32_t const LayerCount;
  uint32_t const GameFeatures;

  bool ShouldQuit = false;

 private:
  Game(GameFeatureConfig const& config);
  void Init();
};

extern Game* g_Game;

}  // namespace Impacto