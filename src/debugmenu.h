#pragma once

#include "impacto.h"

#include <enum.h>

namespace Impacto {
namespace DebugMenu {

extern bool DebugMenuShown;

// This is only for debug info
BETTER_ENUM(ThreadGroupType, uint8_t, Root = 0x0, System = 0x1, GameSys = 0x2,
            GameSys2 = 0x3, GameSys3 = 0x4, Script = 0x5, Script2 = 0x6,
            Script3 = 0x7, Script4 = 0x8, None = 0xFF)

void Show();
void ShowScriptVariablesEditor();
void ShowScriptDebugger();

}  // namespace DebugMenu
}  // namespace Impacto
