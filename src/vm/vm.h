#pragma once

#include "thread.h"

#define VmInstruction(name) void name(Sc3VmThread* thread)

namespace Impacto {
namespace Vm {

typedef void (*InstructionProc)(Sc3VmThread* thread);

int const MaxLoadedScripts = 16;
int const MaxThreads = 100;
int const MaxThreadGroups = 12;

uint8_t* ScriptGetLabelAddress(uint8_t* scriptBufferAdr, uint32_t labelNum);
uint32_t ScriptGetLabelAddressNum(uint8_t* scriptBufferAdr, uint32_t labelNum);
uint8_t* ScriptGetStrAddress(uint8_t* scriptBufferAdr, uint32_t strNum);
uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum);

void Init(uint32_t startScriptId, uint32_t bufferId);
void Update(float dt);

bool LoadScript(uint32_t bufferId, uint32_t scriptId);

Sc3VmThread* CreateThread(uint32_t groupId);
void ControlThreadGroup(ThreadGroupControlType controlType, uint32_t groupId);
void DestroyThread(Sc3VmThread* thread);
void RunThread(Sc3VmThread* thread);

extern uint8_t* ScriptBuffers[MaxLoadedScripts];

extern bool BlockCurrentScriptThread;
extern uint32_t SwitchValue;  // Used in InstSwitch and InstCase

}  // namespace Vm
}  // namespace Impacto