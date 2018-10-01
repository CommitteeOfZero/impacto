#pragma once

#include "3d/scene.h"
#include "impacto.h"

namespace Impacto {

enum GameFeature {
  GameFeature_Nuklear = (1 << 0),
  GameFeature_Scene3D = (1 << 1)
};

struct GameFeatureConfig {
  uint32_t LayerCount;
  uint32_t GameFeatures;

  uint32_t Scene3D_CharacterCount;
  uint32_t Scene3D_BackgroundCount;
};

class BaseGame {
 public:
  void Update(float dt);
  void Render();
  virtual ~BaseGame();

  Scene* Scene3D = 0;
  nk_context* Nk = 0;

  uint32_t const LayerCount;
  uint32_t const GameFeatures;

  bool ShouldQuit = false;

 protected:
  BaseGame(GameFeatureConfig const& config);

  virtual void GameUpdate(float dt) {}

  virtual void DrawLayer(uint32_t layerId) = 0;
};

// NOT set in basegame.cpp!
extern BaseGame* g_Game;

}  // namespace Impacto