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
    ThreadGroupControl[i] = TF_Display;
    ThreadGroupHeads[i] = NULL;
    ThreadGroupTails[i] = NULL;
  }

  memset(&ThreadGroupIds, 0, VmMaxThreadGroups * 4);

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
  ++ThreadGroupIds[groupId];
  thread->VmContext = this;
  thread->GameContext = GameContext;
  return thread;
}

void Vm::CreateThreadExecTable() {
  int tblIndex = 0;

  for (int i = 0; i < VmMaxThreadGroups; i++) {
    if (ThreadGroupControl[i] & TF_Destroy) {
      DestroyThreadGroup(i);
      ThreadGroupControl[i] ^= TF_Destroy;
    } else if (!(ThreadGroupControl[i] & TF_Pause)) {
      Sc3VmThread* groupThread = ThreadGroupHeads[i];
      if (groupThread == NULL) continue;
      do {
        if (groupThread->Flag & TF_Destroy) {
          Sc3VmThread* next = groupThread->NextContext;
          DestroyThread(groupThread);
          groupThread = next;
        } else if (!(groupThread->Flag & TF_Pause)) {
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
    if (ThreadTable[cnt]->Flag & TF_Destroy) {
      DestroyThread(ThreadTable[cnt]);
    }
    cnt++;
  }
}

void Vm::CreateThreadDrawTable() {
  int tblIndex = 0;

  for (int i = 0; i < VmMaxThreadGroups; i++) {
    if (ThreadGroupControl[i] & TF_Display) {
      Sc3VmThread* groupThread = ThreadGroupHeads[i];
      if (groupThread == NULL) continue;
      do {
        if (groupThread->Flag & TF_Display) {
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

void Vm::DrawAllThreads() {}

void Vm::DestroyThread(Sc3VmThread* thread) {
  Sc3VmThread* previous = thread->PreviousContext;
  Sc3VmThread* next = thread->NextContext;
  if (next != 0) {
    thread->NextContext->PreviousContext = previous;
  }
  if (previous != 0) {
    thread->PreviousContext->NextContext = next;
  }
  --ThreadGroupIds[thread->ScriptBufferId];
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

void Vm::RunThread(Sc3VmThread* thread) {
  uint8_t* scrVal;
  uint32_t opcodeGrp;
  uint32_t opcode;
  uint32_t opcodeGrp1;
  uint32_t calDummy;

  ImpLog(LL_Debug, LC_VM, "Running thread ID = %i\n", thread->Id);

  BlockCurrentScriptThread = 0;
  do {
    scrVal = thread->Ip;
    opcodeGrp = *scrVal;
    if ((uint8_t)opcodeGrp == 0xFE) {
      thread->Ip += 1;
      calMain(thread, &calDummy);
    } else {
      opcode = *(scrVal + 1);
      opcodeGrp1 = opcodeGrp & 0x7F;

      ImpLog(LL_Debug, LC_VM, "Opcode: %02X:%02X\n", opcodeGrp1, opcode);

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
  return scriptBufferAdr +
         ((*(uint8_t*)(scriptBufferAdr + 4 * labelNum + 15) << 24) |
          (*(uint8_t*)(scriptBufferAdr + 4 * labelNum + 14) << 16) |
          (*(uint8_t*)(scriptBufferAdr + 4 * labelNum + 13) << 8) |
          *(uint8_t*)(scriptBufferAdr + 4 * labelNum + 12));
}

uint32_t ScriptGetLabelAddressNum(uint8_t* scriptBufferAdr, uint32_t labelNum) {
  return scriptBufferAdr[12] +
         ((scriptBufferAdr[13] +
           ((scriptBufferAdr[14] + (scriptBufferAdr[15] << 8)) << 8))
          << 8);
}

// TODO: Make this sane
uint8_t* ScriptGetStrAddress(uint8_t* scriptBufferAdr, uint32_t mesNum) {
  int v3 = mesNum + ((scriptBufferAdr[5] +
                      ((scriptBufferAdr[6] + (scriptBufferAdr[7] << 8)) << 8))
                     << 6);
  int v4 = (int)&scriptBufferAdr[scriptBufferAdr[4]];
  return &scriptBufferAdr[256 * (*(uint8_t*)(v4 + 4 * v3 + 1) +
                                 ((*(uint8_t*)(v4 + 4 * v3 + 2) +
                                   (*(uint8_t*)(v4 + 4 * v3 + 3) << 8))
                                  << 8)) +
                          *(uint8_t*)(v4 + 4 * v3)];
}

// TODO: Make this sane
uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum) {
  uint32_t v3 =
      retNum + ((scriptBufferAdr[9] +
                 ((scriptBufferAdr[10] + (scriptBufferAdr[11] << 8)) << 8))
                << 6);
  uint64_t v4 = (uint64_t)&scriptBufferAdr[scriptBufferAdr[8]];
  return &scriptBufferAdr[256 * (*(uint8_t*)(v4 + 4 * v3 + 1) +
                                 ((*(uint8_t*)(v4 + 4 * v3 + 2) +
                                   (*(uint8_t*)(v4 + 4 * v3 + 3) << 8))
                                  << 8)) +
                          *(uint8_t*)(v4 + 4 * v3)];
}

}  // namespace Vm

}  // namespace Impacto