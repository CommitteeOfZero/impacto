#pragma once

#include "3d/scene.h"
#include "impacto.h"
#include "modelviewer.h"
#include "vm/vm.h"

namespace Impacto {

enum GameFeature {
  GameFeature_Nuklear = (1 << 0),
  GameFeature_Scene3D = (1 << 1),
  GameFeature_ModelViewer = (1 << 2),
  GameFeature_Sc3VirtualMachine = (1 << 3)
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
  static void InitVmTest();

  ~Game();
  void Update(float dt);
  void Render();

  Scene* Scene3D = 0;
  nk_context* Nk = 0;
  ModelViewer* ModelViewerComponent = 0;
  Vm::Vm* VmComponent = 0;

  uint32_t const LayerCount;
  uint32_t const GameFeatures;

  uint32_t* ScrWork;
  uint16_t DrawComponents[Vm::VmMaxThreads];
  void SetFlag(uint32_t flagId, uint32_t value);
  bool GetFlag(uint32_t flagId);

  bool ShouldQuit = false;

 private:
  Game(GameFeatureConfig const& config);
  void Init();

  uint8_t* FlagWork;
};

extern Game* g_Game;

}  // namespace Impacto