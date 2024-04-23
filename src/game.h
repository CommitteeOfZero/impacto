#pragma once

#include "impacto.h"
#include "vm/vm.h"
#include "io/vfs.h"
#include "text.h"
#include "renderer/renderer.h"
#include <enum.h>

#include <string>

namespace Impacto {

BETTER_ENUM(RendererType, int, OpenGL, Vulkan, DirectX9, LegacyOpenGL);

BETTER_ENUM(GameFeature, int, DebugMenu = (1 << 0), Scene3D = (1 << 1),
            ModelViewer = (1 << 2), Sc3VirtualMachine = (1 << 3),
            Renderer2D = (1 << 4), Input = (1 << 5), Audio = (1 << 6),
            CharacterViewer = (1 << 7), Video = (1 << 8),
            DebugMenuMultiViewport = (1 << 9))

BETTER_ENUM(VideoPlayerType, int32_t, FFmpeg);

BETTER_ENUM(AudioBackendType, int32_t, OpenAL, Dreamcast);

namespace Game {

BETTER_ENUM(DrawComponentType, uint8_t, Text = 0x0, Main = 0x1,
            ExtrasScenes = 0x2, Mask = 0x3, SystemText = 0x3, SaveMenu = 0x4,
            SaveMenu05 = 0x5, SystemIcons = 0x6, TitleMenu = 0x7, Option = 0x9,
            SystemMenu = 0xA, SystemMessage = 0xB, PlayData = 0xC, Album = 0xD,
            ExtrasMusicMode = 0xE, DictionaryMode = 0xF, ExtrasMovieMode = 0x10,
            ExtrasActorsVoice = 0x11, SaveIcon = 0x12,
            GlobalSystemMessage = 0x15, InstallInfo = 0x16,
            SystemMessage17 = 0x17, DebugEditor = 0x1E, None = 0xFF)

void InitFromProfile(std::string const& name);

void Shutdown();

void Update(float dt);
void Render();

extern uint8_t DrawComponents[Vm::MaxThreads];

extern bool ShouldQuit;
}  // namespace Game

}  // namespace Impacto