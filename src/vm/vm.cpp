#include "vm.h"

#include "expression.h"
#include "../log.h"
#include "../io/vfs.h"
#include "../io/io.h"
#include "../game.h"
#include "../mem.h"
#include "../scriptvars.h"
#include "interface/scene3d.h"
#include "interface/background2d.h"
#include "opcodetables_rne.h"
#include "../profile/game.h"
#include "../profile/vm.h"
#include "../window.h"

namespace Impacto {
namespace Vm {

uint8_t* ScriptBuffers[MaxLoadedScripts];
bool BlockCurrentScriptThread;
uint32_t SwitchValue;

static uint32_t LoadedScriptIds[MaxLoadedScripts];

static Sc3VmThread ThreadPool[MaxThreads];  // Main thread pool where all the
                                            // thread objects are stored
static Sc3VmThread*
    ThreadTable[MaxThreads];  // Table of ordered thread pointers
                              // to be executed or "drawn"
static uint32_t ThreadGroupState[MaxThreadGroups];  // Control states for thread
                                                    // groups. Each thread group
                                                    // is a doubly linked list
static uint32_t ThreadGroupCount[MaxThreadGroups];  // Current number of
                                                    // threads in a group
static Sc3VmThread*
    ThreadGroupHeads[MaxThreadGroups];  // Pointers to thread group
                                        // doubly linked list heads
static Sc3VmThread*
    ThreadGroupTails[MaxThreadGroups];  // Pointers to thread group
                                        // doubly linked list tails
static Sc3VmThread* NextFreeThreadCtx;  // Next free thread context in the
                                        // thread pool

static InstructionProc* OpcodeTableSystem;
static InstructionProc* OpcodeTableUser1;
static InstructionProc* OpcodeTableGraph;
static InstructionProc* OpcodeTableGraph3D;

static void CreateThreadExecTable();
static void SortThreadExecTable();
static void CreateThreadDrawTable();
static void SortThreadDrawTable();
static void DrawAllThreads();
static void DestroyScriptThreads(uint32_t scriptBufferId);
static void DestroyThreadGroup(uint32_t groupId);

void Init() {
  ImpLog(LL_Info, LC_VM, "Initializing SC3 virtual machine\n");

  Profile::Vm::Configure();

  switch (Profile::Vm::GameInstructionSet) {
    case InstructionSet::RNE: {
      OpcodeTableSystem = OpcodeTableSystem_RNE;
      OpcodeTableGraph = OpcodeTableGraph_RNE;
      OpcodeTableGraph3D = OpcodeTableGraph3D_RNE;
      OpcodeTableUser1 = OpcodeTableUser1_RNE;
      break;
    }
    default: {
      ImpLog(LL_Fatal, LC_VM, "Unsupported instruction set\n");
      Window::Shutdown();
      break;
    }
  }

  for (int i = 0; i < MaxThreads - 1; i++) {
    memset(&ThreadPool[i], 0, sizeof(Sc3VmThread));
    ThreadPool[i].NextFreeContext = &ThreadPool[i + 1];
    ThreadPool[i].Id = i;
  }

  NextFreeThreadCtx = ThreadPool;
  memset(&ThreadPool[MaxThreads - 1], 0, sizeof(Sc3VmThread));
  ThreadPool[MaxThreads - 1].Id = MaxThreads - 1;

  for (int i = 0; i < MaxThreadGroups; i++) {
    ThreadGroupState[i] = TF_Display;
    ThreadGroupHeads[i] = NULL;
    ThreadGroupTails[i] = NULL;
  }

  memset(&ThreadGroupCount, 0, MaxThreadGroups * 4);

  bool res =
      LoadScript(Profile::Vm::StartScriptBuffer, Profile::Vm::StartScript);
  if (res) {
    Sc3VmThread* startupThd = CreateThread(0);
    startupThd->GroupId = 0;
    startupThd->ScriptBufferId = Profile::Vm::StartScriptBuffer;
    uint8_t* scrBuf = ScriptBuffers[Profile::Vm::StartScriptBuffer];
    startupThd->Ip = ScriptGetLabelAddress(scrBuf, 0);
  }

  ScrWork[2200] = 1;  // Global animation multiplier maybe?... Set in GameInit()
  SetFlag(SF_MESALLSKIP,
          1);  // SF_MESALLSKIP, force skip mode for now
}

bool LoadScript(uint32_t bufferId, uint32_t scriptId) {
  Io::FileMeta meta;
  Io::VfsGetMeta("script", scriptId, &meta);
  ImpLogSlow(LL_Debug, LC_VM, "Loading script \"%s\"\n", meta.FileName.c_str());

  void* file;
  int64_t fileSize;
  IoError err = Io::VfsSlurp("script", scriptId, &file, &fileSize);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_VM, "Could not read script file for %d\n", scriptId);
    return false;
  }
  ScriptBuffers[bufferId] = (uint8_t*)file;
  ScrWork[SW_SCRIPTNO0 + bufferId] = scriptId;
  return true;
}

Sc3VmThread* CreateThread(uint32_t groupId) {
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
  return thread;
}

// This also destroys thread groups and threads that have the TF_Destroy flag
// set before building the execution table
static void CreateThreadExecTable() {
  int tblIndex = 0;

  for (int i = 0; i < MaxThreadGroups; i++) {
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
      } while (groupThread != NULL);
    }
  }
  ThreadTable[tblIndex] = 0;
}

static void SortThreadExecTable() {
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

void Update() {
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

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Interface::UpdateScene3D();
  }
  Interface::UpdateBackground2D();
}

static void CreateThreadDrawTable() {
  int tblIndex = 0;

  for (int i = 0; i < MaxThreadGroups; i++) {
    if (ThreadGroupState[i] & TF_Display) {
      Sc3VmThread* groupThread = ThreadGroupHeads[i];
      if (groupThread == NULL) continue;
      do {
        if (groupThread->Flags & TF_Display) {
          ThreadTable[tblIndex++] = groupThread;
        }
        groupThread = groupThread->NextContext;
      } while (groupThread != NULL);
    }
  }
  ThreadTable[tblIndex] = 0;
}

static void SortThreadDrawTable() {
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

static void DrawAllThreads() {
  CreateThreadDrawTable();
  SortThreadDrawTable();

  memset(Game::DrawComponents, Game::TD_None, sizeof(Game::DrawComponents));

  int cnt = 0;
  while (ThreadTable[cnt]) {
    Game::DrawComponents[cnt] = ThreadTable[cnt]->DrawType;
    cnt++;
  }
}

void DestroyThread(Sc3VmThread* thread) {
  Sc3VmThread* previous = thread->PreviousContext;
  Sc3VmThread* next = thread->NextContext;
  if (next != 0) {
    thread->NextContext->PreviousContext = previous;
  }
  if (previous != 0) {
    thread->PreviousContext->NextContext = next;
  }
  --ThreadGroupCount[thread->GroupId];
  int id = thread->Id;
  memset(thread, 0, sizeof(Sc3VmThread));
  thread->Id = id;
  thread->NextFreeContext = NextFreeThreadCtx;
  NextFreeThreadCtx = thread;
  thread->PreviousContext = previous;
  thread->NextContext = next;
}

static void DestroyScriptThreads(uint32_t scriptBufferId) {
  int cnt = 0;
  while (ThreadTable[cnt]) {
    if (ThreadTable[cnt]->ScriptBufferId == scriptBufferId) {
      DestroyThread(ThreadTable[cnt]);
    }
    cnt++;
  }
}

static void DestroyThreadGroup(uint32_t groupId) {
  Sc3VmThread* groupThread = ThreadGroupHeads[groupId];
  if (groupThread != NULL) {
    while (groupThread->NextContext != NULL) {
      Sc3VmThread* next = groupThread->NextContext;
      DestroyThread(groupThread);
      groupThread = next;
    }
  }
}

void ControlThreadGroup(ThreadGroupControlType controlType, uint32_t groupId) {
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

void RunThread(Sc3VmThread* thread) {
  uint8_t* scrVal;
  uint32_t opcodeGrp;
  uint32_t opcode;
  uint32_t opcodeGrp1;
  int calDummy;

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
  uint32_t stringTableAdrRel = SDL_SwapLE32(*(uint32_t*)&scriptBufferAdr[4]);
  uint32_t* stringTableAdr = (uint32_t*)&scriptBufferAdr[stringTableAdrRel];
  uint32_t stringAdrRel = SDL_SwapLE32(stringTableAdr[mesNum]);
  return &scriptBufferAdr[stringAdrRel];
}

uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum) {
  uint32_t returnTableAdrRel = SDL_SwapLE32(*(uint32_t*)&scriptBufferAdr[8]);
  uint32_t* returnTableAdr = (uint32_t*)&scriptBufferAdr[returnTableAdrRel];
  uint32_t returnAdrRel = SDL_SwapLE32(returnTableAdr[retNum]);
  return &scriptBufferAdr[returnAdrRel];
}

}  // namespace Vm
}  // namespace Impacto