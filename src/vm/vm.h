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
uint32_t ScriptGetLabelAddressNum(uint8_t* scriptBufferAdr, uint32_t labelNum);
uint8_t* ScriptGetStrAddress(uint8_t* scriptBufferAdr, uint32_t strNum);
uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum);
void* GetMemberPointer(Sc3VmThread* thd, uint32_t offset);

class Vm {
 public:
  Vm(Game* gameCtx);

  void Init(uint32_t startScriptId, uint32_t bufferId);
  void Update();

  uint8_t* ScriptBuffers[VmMaxLoadedScripts];
  bool LoadScript(uint32_t bufferId, uint32_t scriptId);

  Sc3VmThread* CreateThread(uint32_t groupId);
  void ControlThreadGroup(uint32_t controlType, uint32_t groupId);
  void DestroyThread(Sc3VmThread* thread);

 private:
  Game* GameContext;

  bool BlockCurrentScriptThread;
  uint32_t LoadedScriptIds[VmMaxLoadedScripts];

  Sc3VmThread ThreadPool[VmMaxThreads];    // Main thread pool where all the
                                           // thread objects are stored
  Sc3VmThread* ThreadTable[VmMaxThreads];  // Table of ordered thread pointers
                                           // to be executed or "drawn"
  uint32_t ThreadGroupControl[VmMaxThreadGroups];  // Control states for thread
                                                   // groups. Each thread group
                                                   // is a doubly linked list
  uint32_t ThreadGroupCount[VmMaxThreadGroups];  // Current number of threads in
                                                 // a group
  Sc3VmThread* ThreadGroupHeads[VmMaxThreadGroups];  // Pointers to thread group
                                                     // doubly linked list heads
  Sc3VmThread* ThreadGroupTails[VmMaxThreadGroups];  // Pointers to thread group
                                                     // doubly linked list tails
  Sc3VmThread* NextFreeThreadCtx;  // Next free thread context in the thread
                                   // pool

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