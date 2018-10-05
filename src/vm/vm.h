#pragma once

#include "thread.h"

#define VmInstruction(name) void name(Sc3VmThread* thread)

namespace Impacto {

namespace Vm {

typedef void (*InstructionProc)(Sc3VmThread* thread);

int const VmMaxLoadedScripts = 16;
int const VmMaxThreads = 100;
int const VmMaxThreadGroups = 12;

uint8_t* ScriptGetLabelAddress(uint8_t* scriptBufferAdr, uint32_t labelNum);
uint8_t* ScriptGetStrAddress(uint8_t* scriptBufferAdr, uint32_t strNum);
uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum);

class Vm {
 public:
  Vm(Game* gameCtx);

  void Init(uint32_t startScriptId, uint32_t bufferId);
  void Update();

  bool LoadScript(uint32_t bufferId, uint32_t scriptId);

  Sc3VmThread* CreateThread(uint32_t groupId);
  void ControlThreadGroup(uint32_t controlType, uint32_t groupId);
  void DestroyThread(Sc3VmThread* thread);

 private:
  Game* GameContext;

  bool BlockCurrentScriptThread;
  uint8_t* LoadedScriptAdrs[VmMaxLoadedScripts];
  uint32_t LoadedScriptIds[VmMaxLoadedScripts];

  uint8_t* ScriptBuffers[VmMaxLoadedScripts];

  Sc3VmThread ThreadPool[VmMaxThreads];
  Sc3VmThread* ThreadTable[VmMaxThreads];
  uint32_t ThreadGroupControl[VmMaxThreadGroups];
  uint32_t ThreadGroupIds[VmMaxThreadGroups];
  // Sc3VmThread ThreadGroupIndex[VmMaxThreadGroups * 2];
  Sc3VmThread* ThreadGroupHeads[VmMaxThreadGroups];
  Sc3VmThread* ThreadGroupTails[VmMaxThreadGroups];
  Sc3VmThread* NextFreeThreadCtx;

  InstructionProc* OpcodeTableSystem;
  InstructionProc* OpcodeTableUser1;
  InstructionProc* OpcodeTableGraph;
  InstructionProc* OpcodeTableGraph3D;

  void CreateThreadExecTable();
  void SortThreadExecTable();
  void CreateThreadDrawTable();
  void SortThreadDrawTable();
  void DrawAllThreads();
  void DestroyScriptThreads(uint32_t scriptBufferId);
  void DestroyThreadGroup(uint32_t groupId);
  void RunThread(Sc3VmThread* thread);
};

}  // namespace Vm

}  // namespace Impacto