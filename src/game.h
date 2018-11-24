#pragma once

#include "impacto.h"
#include "vm/vm.h"
#include "io/vfs.h"
#include "text.h"
#include <enum.h>

#include <string>

namespace Impacto {

extern nk_context* Nk;

BETTER_ENUM(GameFeature, int, Nuklear = (1 << 0), Scene3D = (1 << 1),
            ModelViewer = (1 << 2), Sc3VirtualMachine = (1 << 3),
            Renderer2D = (1 << 4), Input = (1 << 5), Audio = (1 << 6))

namespace Profile {
extern DialoguePageFeatureConfig Dlg;
}  // namespace Profile

namespace Game {

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

void InitFromProfile(std::string const& name);

void Shutdown();

void Update(float dt);
void Render();

extern DrawComponentType DrawComponents[Vm::MaxThreads];

extern bool ShouldQuit;
}  // namespace Game

}  // namespace Impacto