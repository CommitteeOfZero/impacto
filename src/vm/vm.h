#pragma once

#include <span>
#include "thread.h"
#include "../io/vfs.h"
#include <enum.h>

#define VmInstruction(name) void name(Sc3VmThread* thread)

namespace Impacto {
namespace Vm {

BETTER_ENUM(InstructionSet, int, RNE, Darling, CHLCC, MO6TW, MO7, Dash, CC,
            SGPS3, MO8, CHN)

using InstructionProc = auto (*)(Sc3VmThread* thread) -> void;

int constexpr MaxLoadedScripts = 16;
int constexpr MaxThreads = 100;
int constexpr MaxThreadGroups = 12;

uint32_t ScriptGetLabelSize(uint32_t scriptBufferId, uint32_t labelNum);
uint32_t ScriptGetLabelAddress(uint32_t scriptBufferId, uint32_t labelNum);
uint32_t ScriptGetStrAddress(uint32_t scriptBufferId, uint32_t strNum);

BufferOffsetContext ScriptGetTextTableStrAddress(uint32_t textTableId,
                                                 uint32_t strNum);
uint32_t ScriptGetRetAddress(uint32_t scriptBufferId, uint32_t retNum);
uint32_t MsbGetStrAddress(uint32_t scriptBufferId, uint32_t mesNum);

void Init();
void Update(float dt);

bool LoadScript(uint32_t bufferId, uint32_t scriptId);
bool LoadMsb(uint32_t bufferId, uint32_t fileId);

Sc3VmThread* CreateThread(uint32_t groupId);
void ControlThreadGroup(ThreadGroupControlType controlType, uint32_t groupId);
void DestroyThread(Sc3VmThread* thread);
void RunThread(Sc3VmThread* thread);

inline std::span<uint8_t> ScriptBuffers[MaxLoadedScripts];
inline std::span<uint8_t> MsbBuffers[MaxLoadedScripts];

inline Io::FileMeta LoadedScriptMetas[MaxLoadedScripts];

inline Sc3VmThread ThreadPool[MaxThreads];  // Main thread pool where all the
                                            // thread objects are stored

inline bool BlockCurrentScriptThread;
inline uint32_t SwitchValue;  // Used in InstSwitch and InstCase

struct TextTableEntry {
  uint8_t scriptBufferId;
  uint32_t labelAdr;
};

inline TextTableEntry TextTable[16];

#ifndef IMPACTO_DISABLE_IMGUI
inline uint32_t DebugThreadId = std::numeric_limits<uint32_t>::max();
inline bool DebuggerBreak = false;
inline bool DebuggerStepRequest = false;
inline bool DebuggerContinueRequest = false;
inline bool DebuggerAlwaysBlock = false;
inline std::map<int, std::pair<uint32_t, uint32_t>> DebuggerBreakpoints;
#endif

}  // namespace Vm
}  // namespace Impacto