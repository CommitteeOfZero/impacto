#pragma once

#include "thread.h"
#include "../io/vfs.h"
#include <enum.h>

#define VmInstruction(name) void name(Sc3VmThread* thread)

namespace Impacto {
namespace Vm {

BETTER_ENUM(InstructionSet, int, RNE, Darling, CHLCC, MO6TW, MO7, Dash, CC,
            SGPS3, MO8, CHN)

typedef void (*InstructionProc)(Sc3VmThread* thread);

int const MaxLoadedScripts = 16;
int const MaxThreads = 100;
int const MaxThreadGroups = 12;

uint8_t* ScriptGetLabelAddress(uint8_t* scriptBufferAdr, uint32_t labelNum);
uint32_t ScriptGetLabelAddressNum(uint8_t* scriptBufferAdr, uint32_t labelNum);
uint8_t* ScriptGetStrAddress(uint8_t* scriptBufferAdr, uint32_t strNum);
uint8_t* ScriptGetTextTableStrAddress(uint32_t textTableId, uint32_t strNum);
uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum);
uint8_t* MsbGetStrAddress(uint8_t* msbBufferAdr, uint32_t mesNum);

void Init();
void Update();

bool LoadScript(uint32_t bufferId, uint32_t scriptId);
bool LoadMsb(uint32_t bufferId, uint32_t fileId);

Sc3VmThread* CreateThread(uint32_t groupId);
void ControlThreadGroup(ThreadGroupControlType controlType, uint32_t groupId);
void DestroyThread(Sc3VmThread* thread);
void RunThread(Sc3VmThread* thread);

extern uint8_t* ScriptBuffers[MaxLoadedScripts];
extern uint8_t* MsbBuffers[MaxLoadedScripts];

extern Io::FileMeta LoadedScriptMetas[MaxLoadedScripts];

extern Sc3VmThread ThreadPool[MaxThreads];

extern bool BlockCurrentScriptThread;
extern uint32_t SwitchValue;  // Used in InstSwitch and InstCase

struct TextTableEntry {
  uint8_t* scriptBufferAdr;
  uint8_t* labelAdr;
};

extern TextTableEntry TextTable[16];

#ifndef IMPACTO_DISABLE_IMGUI
extern uint32_t DebugThreadId;
extern bool DebuggerBreak;
extern bool DebuggerStepRequest;
extern bool DebuggerContinueRequest;
extern std::map<int, uint32_t> DebuggerBreakpoints;
#endif

}  // namespace Vm
}  // namespace Impacto