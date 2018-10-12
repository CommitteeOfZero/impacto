#pragma once

#include "3d/scene.h"
#include "impacto.h"
#include "modelviewer.h"
#include "vm/vm.h"
#include "renderer2d.h"
#include "io/vfs.h"

#include "text.h"

namespace Impacto {

enum GameFeature {
  GameFeature_Nuklear = (1 << 0),
  GameFeature_Scene3D = (1 << 1),
  GameFeature_ModelViewer = (1 << 2),
  GameFeature_Sc3VirtualMachine = (1 << 3),
  GameFeature_Renderer2D = (1 << 4)
};

struct GameFeatureConfig {
  uint32_t LayerCount;
  uint32_t GameFeatures;

  std::string SystemArchiveName = "";

  uint32_t Scene3D_CharacterCount;
  uint32_t Scene3D_BackgroundCount;

  DialoguePageFeatureConfig Dlg;
};

class Game {
 public:
  static Game* CreateModelViewer();
  static Game* CreateVmTest();
  static Game* CreateDialogueTest();

  ~Game();
  void Update(float dt);
  void Render();

  void SetFlag(uint32_t flagId, uint32_t value);
  bool GetFlag(uint32_t flagId);

  Scene* Scene3D = 0;
  nk_context* Nk = 0;
  ModelViewer* ModelViewerComponent = 0;
  Vm::Vm* VmComponent = 0;
  Renderer2D* R2D = 0;

  GameFeatureConfig const Config;

  Font MainFont;

  uint32_t* ScrWork;
  DrawComponentType DrawComponents[Vm::VmMaxThreads];

  VfsArchive* SystemArchive = 0;

  bool ShouldQuit = false;

 private:
  Game(GameFeatureConfig const& config);
  void Init();

  uint8_t* FlagWork;
};

enum DrawComponentType : uint8_t {
  TD_Text = 0x0,
  TD_Main = 0x1,
  TD_ExtrasScenes = 0x2,
  TD_Mask = 0x3,
  TD_SystemText = 0x4,
  TD_SaveMenu = 0x5,
  TD_SystemIcons = 0x6,
  TD_TitleMenu = 0x7,
  TD_Option = 0x9,
  TD_SystemMenu = 0xA,
  TD_SystemMessage = 0xB,
  TD_PlayData = 0xC,
  TD_Album = 0xD,
  TD_ExtrasMusicMode = 0xE,
  TD_ExtrasMovieMode = 0x10,
  TD_SaveIcon = 0x12,
  TD_GlobalSystemMessage = 0x15,
  TD_DebugEditor = 0x1E,

  TD_None = 0xFF
};

}  // namespace Impacto