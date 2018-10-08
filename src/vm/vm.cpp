#include "vm.h"

#include "expression.h"
#include "../log.h"
#include "../io/vfs.h"
#include "../io/io.h"
#include "../game.h"

#include "opcodetables_rne.h"

namespace Impacto {

namespace Vm {

static VfsArchive* AllScriptsArchive = NULL;

Vm::Vm(Game* gameCtx) {
  GameContext = gameCtx;
  OpcodeTableSystem = OpcodeTableSystem_RNE;
  OpcodeTableGraph = OpcodeTableGraph_RNE;
  OpcodeTableGraph3D = OpcodeTableGraph3D_RNE;
  OpcodeTableUser1 = OpcodeTableUser1_RNE;
}

void Vm::Init(uint32_t startScriptId, uint32_t bufferId) {
  ImpLog(LL_Info, LC_VM, "Initializing SC3 virtual machine\n");

  for (int i = 0; i < VmMaxThreads - 1; i++) {
    memset(&ThreadPool[i], 0, sizeof(Sc3VmThread));
    ThreadPool[i].NextFreeContext = &ThreadPool[i + 1];
  }

  NextFreeThreadCtx = ThreadPool;
  memset(&ThreadPool[VmMaxThreads - 1], 0, sizeof(Sc3VmThread));

  for (int i = 0; i < VmMaxThreadGroups; i++) {
    ThreadGroupState[i] = TF_Display;
    ThreadGroupHeads[i] = NULL;
    ThreadGroupTails[i] = NULL;
  }

  memset(&ThreadGroupCount, 0, VmMaxThreadGroups * 4);

  IoError err = VfsArchive::Mount("script.cpk", &AllScriptsArchive);
  if (err != IoError_OK) {
    AllScriptsArchive = NULL;
    ImpLog(LL_Error, LC_VM, "Could not open script archive: %d\n", err);
  }

  bool res = LoadScript(bufferId, startScriptId);
  if (res) {
    Sc3VmThread* startupThd = CreateThread(0);
    startupThd->ScriptBufferId = bufferId;
    uint8_t* scrBuf = ScriptBuffers[bufferId];
    startupThd->Ip = ScriptGetLabelAddress(scrBuf, 0);
  }
}

bool Vm::LoadScript(uint32_t bufferId, uint32_t scriptId) {
  assert(AllScriptsArchive != NULL);
  char scriptName[256];
  AllScriptsArchive->GetName(scriptId, scriptName);
  ImpLogSlow(LL_Debug, LC_VM, "Loading script \"%s\"\n", scriptName);

  void* file;
  int64_t fileSize;
  IoError err = AllScriptsArchive->Slurp(scriptId, &file, &fileSize);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_VM, "Could not read script file for %d\n", scriptId);
    return false;
  }
  ScriptBuffers[bufferId] = (uint8_t*)file;
  return true;
}

Sc3VmThread* Vm::CreateThread(uint32_t groupId) {
  if (!NextFreeThreadCtx) return 0;
  Sc3VmThread* thread = NextFreeThreadCtx;
  NextFreeThreadCtx = NextFreeThreadCtx->NextFreeContext;
  int id = thread->Id;
  memset(thread, 0, sizeof(Sc3VmThread));
  thread->Id = id;
  if (ThreadGroupHeads[groupId] == NULL) {
    ThreadGroupHeads[groupId] = thread;
    ThreadGroupTails[groupId] = thread;
  } else {
    Sc3VmThread* prevGroupTail = ThreadGroupTails[groupId];
    thread->PreviousContext = prevGroupTail;
    prevGroupTail->NextContext = thread;
    ThreadGroupTails[groupId] = thread;
  }
  ++ThreadGroupCount[groupId];
  thread->VmContext = this;
  thread->GameContext = GameContext;
  return thread;
}

// This also destroys thread groups and threads that have the TF_Destroy flag
// set before building the execution table
void Vm::CreateThreadExecTable() {
  int tblIndex = 0;

  for (int i = 0; i < VmMaxThreadGroups; i++) {
    if (ThreadGroupState[i] & TF_Destroy) {
      DestroyThreadGroup(i);
      ThreadGroupState[i] ^= TF_Destroy;
    } else if (!(ThreadGroupState[i] & TF_Pause)) {
      Sc3VmThread* groupThread = ThreadGroupHeads[i];
      if (groupThread == NULL) continue;
      do {
        if (groupThread->Flags & TF_Destroy) {
          Sc3VmThread* next = groupThread->NextContext;
          DestroyThread(groupThread);
          groupThread = next;
        } else if (!(groupThread->Flags & TF_Pause)) {
          ThreadTable[tblIndex++] = groupThread;
          groupThread = groupThread->NextContext;
        } else {
          groupThread = groupThread->NextContext;
        }
      } while (groupThread != NULL && groupThread->NextContext != NULL);
    }
  }
  ThreadTable[tblIndex] = 0;
}

void Vm::SortThreadExecTable() {
  int i = 0;
  while (ThreadTable[i]) {
    int j = 0;
    while (ThreadTable[j + i] && ThreadTable[j + 1]) {
      if (ThreadTable[j]->ExecPriority > ThreadTable[j + 1]->ExecPriority) {
        Sc3VmThread* temp = ThreadTable[j];
        ThreadTable[j] = ThreadTable[j + 1];
        ThreadTable[j + 1] = temp;
      }
      j++;
    }
    i++;
  }
}

void Vm::Update() {
  CreateThreadExecTable();
  SortThreadExecTable();

  int cnt = 0;
  while (ThreadTable[cnt]) {
    RunThread(ThreadTable[cnt++]);
  }

  cnt = 0;
  while (ThreadTable[cnt]) {
    if (ThreadTable[cnt]->Flags & TF_Destroy) {
      DestroyThread(ThreadTable[cnt]);
    }
    cnt++;
  }

  DrawAllThreads();
}

void Vm::CreateThreadDrawTable() {
  int tblIndex = 0;

  for (int i = 0; i < VmMaxThreadGroups; i++) {
    if (ThreadGroupState[i] & TF_Display) {
      Sc3VmThread* groupThread = ThreadGroupHeads[i];
      if (groupThread == NULL) continue;
      do {
        if (groupThread->Flags & TF_Display) {
          ThreadTable[tblIndex++] = groupThread;
        }
        groupThread = groupThread->NextContext;
      } while (groupThread != NULL && groupThread->NextContext != NULL);
    }
  }
  ThreadTable[tblIndex] = 0;
}

void Vm::SortThreadDrawTable() {
  int i = 0;
  while (ThreadTable[i]) {
    int j = 0;
    while (ThreadTable[j + i] && ThreadTable[j + 1]) {
      if (ThreadTable[j]->DrawPriority > ThreadTable[j + 1]->DrawPriority) {
        Sc3VmThread* temp = ThreadTable[j];
        ThreadTable[j] = ThreadTable[j + 1];
        ThreadTable[j + 1] = temp;
      }
      j++;
    }
    i++;
  }
}

void Vm::DrawAllThreads() {
  CreateThreadDrawTable();
  SortThreadDrawTable();

  int cnt = 0;
  while (ThreadTable[cnt]) {
    GameContext->DrawComponents[cnt] = ThreadTable[cnt]->DrawType;
    cnt++;
  }
}

void Vm::DestroyThread(Sc3VmThread* thread) {
  Sc3VmThread* previous = thread->PreviousContext;
  Sc3VmThread* next = thread->NextContext;
  if (next != 0) {
    thread->NextContext->PreviousContext = previous;
  }
  if (previous != 0) {
    thread->PreviousContext->NextContext = next;
  }
  --ThreadGroupCount[thread->ScriptBufferId];
  int id = thread->Id;
  memset(thread, 0, sizeof(Sc3VmThread));
  thread->Id = id;
  thread->NextFreeContext = NextFreeThreadCtx;
  NextFreeThreadCtx = thread;
  thread->PreviousContext = previous;
  thread->NextContext = next;
}

void Vm::DestroyScriptThreads(uint32_t scriptBufferId) {
  int cnt = 0;
  while (ThreadTable[cnt]) {
    if (ThreadTable[cnt]->ScriptBufferId == scriptBufferId) {
      DestroyThread(ThreadTable[cnt]);
    }
    cnt++;
  }
}

void Vm::DestroyThreadGroup(uint32_t groupId) {
  Sc3VmThread* groupThread = ThreadGroupHeads[groupId];
  while (groupThread->NextContext != 0) {
    Sc3VmThread* next = groupThread->NextContext;
    DestroyThread(groupThread);
    groupThread = next;
  }
}

void Vm::ControlThreadGroup(ThreadGroupControlType controlType,
                            uint32_t groupId) {
  switch (controlType) {
    case TC_Destroy:
      ThreadGroupState[groupId] |= TF_Destroy;
      break;
    case TC_Pause:
      ThreadGroupState[groupId] |= TF_Pause;
      break;
    case TC_Start:
      ThreadGroupState[groupId] =
          (ThreadGroupState[groupId] | TF_Pause) ^ TF_Pause;
      break;
    case TC_Hide:
      ThreadGroupState[groupId] =
          (ThreadGroupState[groupId] | TF_Display) ^ TF_Display;
      break;
    case TC_Display:
      ThreadGroupState[groupId] |= TF_Display;
      break;
    default:
      break;
  }
}

void Vm::RunThread(Sc3VmThread* thread) {
  uint8_t* scrVal;
  uint32_t opcodeGrp;
  uint32_t opcode;
  uint32_t opcodeGrp1;
  uint32_t calDummy;

  ImpLog(LL_Trace, LC_VM, "Running thread ID = %i\n", thread->Id);

  BlockCurrentScriptThread = 0;
  do {
    scrVal = thread->Ip;
    opcodeGrp = *scrVal;
    if ((uint8_t)opcodeGrp == 0xFE) {
      thread->Ip += 1;
      ExpressionEval(thread, &calDummy);
    } else {
      opcode = *(scrVal + 1);
      opcodeGrp1 = opcodeGrp & 0x7F;

      ImpLog(LL_Trace, LC_VM, "Opcode: %02X:%02X\n", opcodeGrp1, opcode);

      if (opcodeGrp1 == 0x10) {
        OpcodeTableUser1[opcode](thread);
      } else if (opcodeGrp1 == 0x02) {
        OpcodeTableGraph3D[opcode](thread);
      } else if (opcodeGrp1 == 0x01) {
        OpcodeTableGraph[opcode](thread);
      } else if (!opcodeGrp1) {
        OpcodeTableSystem[opcode](thread);
      }
    }
  } while (!BlockCurrentScriptThread);
}

uint8_t* ScriptGetLabelAddress(uint8_t* scriptBufferAdr, uint32_t labelNum) {
  uint32_t* labelTableAdr = (uint32_t*)&scriptBufferAdr[12];
  uint32_t labelAdrRel = SDL_SwapLE32(labelTableAdr[labelNum]);
  return &scriptBufferAdr[labelAdrRel];
}

uint32_t ScriptGetLabelAddressNum(uint8_t* scriptBufferAdr, uint32_t labelNum) {
  uint32_t* labelTableAdr = (uint32_t*)&scriptBufferAdr[12];
  uint32_t labelAdrRel = SDL_SwapLE32(labelTableAdr[labelNum]);
  return labelAdrRel;
}

uint8_t* ScriptGetStrAddress(uint8_t* scriptBufferAdr, uint32_t mesNum) {
  uint32_t stringTableAdrRel = SDL_SwapLE32(*(uint32_t*)scriptBufferAdr[4]);
  uint32_t* stringTableAdr = (uint32_t*)&scriptBufferAdr[stringTableAdrRel];
  uint32_t stringAdrRel = SDL_SwapLE32(stringTableAdr[mesNum]);
  return &scriptBufferAdr[stringAdrRel];
}

uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum) {
  uint32_t returnTableAdrRel = SDL_SwapLE32(*(uint32_t*)scriptBufferAdr[8]);
  uint32_t* returnTableAdr = (uint32_t*)&scriptBufferAdr[returnTableAdrRel];
  uint32_t returnAdrRel = SDL_SwapLE32(returnTableAdr[retNum]);
  return &scriptBufferAdr[returnAdrRel];
}

}  // namespace Vm

}  // namespace Impacto