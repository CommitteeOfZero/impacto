#include "vm.h"

#include "expression.h"
#include "../log.h"
#include "../io/io.h"
#include "../game.h"
#include "../mem.h"
#include "interface/scene3d.h"
#include "interface/scene2d.h"
#include "interface/input.h"
#include "opcodetables_rne.h"
#include "opcodetables_darling.h"
#include "opcodetables_chlcc.h"
#include "opcodetables_mo6tw.h"
#include "opcodetables_mo7.h"
#include "opcodetables_mo8.h"
#include "opcodetables_dash.h"
#include "opcodetables_cc.h"
#include "opcodetables_sgps3.h"
#include "opcodetables_chn.h"
#include "../profile/game.h"
#include "../profile/vm.h"
#include "../profile/scriptinput.h"
#include "../profile/scriptvars.h"
#include "../profile/dialogue.h"

namespace Impacto {
namespace Vm {

using namespace Profile::ScriptVars;

uint8_t* ScriptBuffers[MaxLoadedScripts];
uint8_t* MsbBuffers[MaxLoadedScripts];
Io::FileMeta LoadedScriptMetas[MaxLoadedScripts];
bool BlockCurrentScriptThread;
uint32_t SwitchValue;
TextTableEntry TextTable[16];

#ifndef IMPACTO_DISABLE_IMGUI
uint32_t DebugThreadId = -1;
bool DebuggerBreak = false;
bool DebuggerStepRequest = false;
bool DebuggerContinueRequest = false;
std::map<int, uint32_t> DebuggerBreakpoints;
#endif

Sc3VmThread ThreadPool[MaxThreads];  // Main thread pool where all the
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
// TODO: Make this static. It's only global to silence the unused function
// warning, as it may be required for future games.
// Alternatively, this could be marked
// [[maybe_unused]] once the project has been upgraded to C++17.
void DestroyScriptThreads(uint32_t scriptBufferId);
static void DestroyThreadGroup(uint32_t groupId);

void Init() {
  ImpLog(LL_Info, LC_VM,
         "Initializing SC3 virtual machine\n**** Start apprication ****\n");

  Profile::Vm::Configure();
  Profile::ScriptInput::Configure();
  Profile::ScriptVars::Configure();

  switch (Profile::Vm::GameInstructionSet) {
    case InstructionSet::RNE: {
      OpcodeTableSystem = OpcodeTableSystem_RNE;
      OpcodeTableGraph = OpcodeTableGraph_RNE;
      OpcodeTableGraph3D = OpcodeTableGraph3D_RNE;
      OpcodeTableUser1 = OpcodeTableUser1_RNE;
      break;
    }
    case InstructionSet::Darling: {
      OpcodeTableSystem = OpcodeTableSystem_Darling;
      OpcodeTableGraph = OpcodeTableGraph_Darling;
      OpcodeTableUser1 = OpcodeTableUser1_Darling;
      break;
    }
    case InstructionSet::CHLCC: {
      OpcodeTableSystem = OpcodeTableSystem_CHLCC;
      OpcodeTableGraph = OpcodeTableGraph_CHLCC;
      OpcodeTableUser1 = OpcodeTableUser1_CHLCC;
      break;
    }
    case InstructionSet::MO6TW: {
      OpcodeTableSystem = OpcodeTableSystem_MO6TW;
      OpcodeTableGraph = OpcodeTableGraph_MO6TW;
      OpcodeTableUser1 = OpcodeTableUser1_MO6TW;
      break;
    }
    case InstructionSet::MO7: {
      OpcodeTableSystem = OpcodeTableSystem_MO7;
      OpcodeTableGraph = OpcodeTableGraph_MO7;
      OpcodeTableUser1 = OpcodeTableUser1_MO7;
      break;
    }
    case InstructionSet::Dash: {
      OpcodeTableSystem = OpcodeTableSystem_Dash;
      OpcodeTableGraph = OpcodeTableGraph_Dash;
      OpcodeTableGraph3D = OpcodeTableGraph3D_Dash;
      OpcodeTableUser1 = OpcodeTableUser1_Dash;
      break;
    }
    case InstructionSet::CC: {
      OpcodeTableSystem = OpcodeTableSystem_CC;
      OpcodeTableGraph = OpcodeTableGraph_CC;
      OpcodeTableUser1 = OpcodeTableUser1_CC;
      break;
    }
    case InstructionSet::SGPS3: {
      OpcodeTableSystem = OpcodeTableSystem_SGPS3;
      OpcodeTableGraph = OpcodeTableGraph_SGPS3;
      OpcodeTableUser1 = OpcodeTableUser1_SGPS3;
      break;
    }
    case InstructionSet::MO8: {
      OpcodeTableSystem = OpcodeTableSystem_MO8;
      OpcodeTableGraph = OpcodeTableGraph_MO8;
      OpcodeTableUser1 = OpcodeTableUser1_MO8;
      break;
    }
    case InstructionSet::CHN: {
      OpcodeTableSystem = OpcodeTableSystem_CHN;
      OpcodeTableGraph = OpcodeTableGraph_CHN;
      OpcodeTableUser1 = OpcodeTableUser1_CHN;
      break;
    }
    default: {
      ImpLog(LL_Fatal, LC_VM, "Unsupported instruction set\n");
      Window->Shutdown();
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
  if (Profile::Vm::UseMsbStrings) {
    res = LoadMsb(Profile::Vm::StartScriptBuffer,
                  Profile::Vm::UseSeparateMsbArchive
                      ? Profile::Vm::StartScript
                      : Profile::Vm::StartScript - 1);
  }
  if (res) {
    Sc3VmThread* startupThd = CreateThread(0);
    startupThd->GroupId = 0;
    startupThd->ScriptBufferId = Profile::Vm::StartScriptBuffer;
    uint8_t* scrBuf = ScriptBuffers[Profile::Vm::StartScriptBuffer];
    startupThd->Ip = ScriptGetLabelAddress(scrBuf, 0);
  }

  ScrWork[2200] = 1;  // Global animation multiplier maybe?... Set in GameInit()
  // SetFlag(SF_MESALLSKIP,
  //        1);  // Force skip mode for now
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
  LoadedScriptMetas[bufferId] = meta;
  return true;
}

bool LoadMsb(uint32_t bufferId, uint32_t fileId) {
  Io::FileMeta meta;
  std::string mountPoint =
      Profile::Vm::UseSeparateMsbArchive ? "mes" : "script";
  Io::VfsGetMeta(mountPoint.c_str(), fileId, &meta);
  ImpLogSlow(LL_Debug, LC_VM, "Loading msb file \"%s\"\n",
             meta.FileName.c_str());

  void* file;
  int64_t fileSize;
  IoError err = Io::VfsSlurp(mountPoint.c_str(), fileId, &file, &fileSize);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_VM, "Could not read msb file for %d\n", fileId);
    return false;
  }
  MsbBuffers[bufferId] = (uint8_t*)file;
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
  Interface::UpdatePADInput();

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
  } else {
    Interface::UpdateCharacter2D();
  }
  if (Profile::Dialogue::HasSpeakerPortraits) {
    Interface::UpdateSpeakerPortraits();
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

  memset(Game::DrawComponents, Game::DrawComponentType::None,
         sizeof(Game::DrawComponents));

  int cnt = 0;
  while (ThreadTable[cnt]) {
    if (ThreadTable[cnt]->Flags & TF_Display) {
      Game::DrawComponents[cnt] = ThreadTable[cnt]->DrawType;
    }
    cnt++;
  }
}

void DestroyThread(Sc3VmThread* thread) {
  if (ThreadGroupHeads[thread->GroupId] == thread) {
    ThreadGroupHeads[thread->GroupId] = thread->NextContext;
  } else if (ThreadGroupTails[thread->GroupId] == thread) {
    ThreadGroupTails[thread->GroupId] = thread->PreviousContext;
  }
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

void DestroyScriptThreads(uint32_t scriptBufferId) {
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
  ThreadGroupHeads[groupId] = NULL;
  ThreadGroupTails[groupId] = NULL;
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

#ifndef IMPACTO_DISABLE_IMGUI
  if (DebugThreadId == thread->Id) {
    if (DebuggerBreak && !DebuggerStepRequest && !DebuggerContinueRequest)
      return;
  }
#endif

  BlockCurrentScriptThread = 0;
  do {
#ifndef IMPACTO_DISABLE_IMGUI
    if (DebugThreadId == thread->Id) {
      auto scriptIp = thread->Ip - ScriptBuffers[thread->ScriptBufferId];
      for (auto breakpoint : DebuggerBreakpoints) {
        if (scriptIp == breakpoint.second && !DebuggerStepRequest &&
            !DebuggerContinueRequest) {
          DebuggerBreak = true;
          return;
        }
      }
      DebuggerStepRequest = false;
      if (DebuggerContinueRequest) {
        DebuggerContinueRequest = false;
        DebuggerBreak = false;
      }
    }
#endif

    scrVal = thread->Ip;
    opcodeGrp = *scrVal;
    if ((uint8_t)opcodeGrp == 0xFE) {
      thread->Ip += 1;
      ExpressionEval(thread, &calDummy);
    } else {
      opcode = *(scrVal + 1);
      opcodeGrp1 = opcodeGrp & 0x7F;

      ImpLog(LL_Trace, LC_VM,
             "Address: %016X Opcode: %02X:%02X ScriptBuffer: %i\n", thread->Ip,
             opcodeGrp1, opcode, thread->ScriptBufferId);

      if (opcodeGrp1 == 0x10) {
        OpcodeTableUser1[opcode](thread);
      } else if (opcodeGrp1 == 0x02) {
        OpcodeTableGraph3D[opcode](thread);
      } else if (opcodeGrp1 == 0x01) {
        OpcodeTableGraph[opcode](thread);
      } else if (!opcodeGrp1) {
        OpcodeTableSystem[opcode](thread);
      } else {
        ImpLog(LL_Error, LC_VM,
               "Thread CRASH! Unknown opcode. Attempting recovery. Address: "
               "%016X Opcode: %02X:%02X ScriptBuffer: %i\n",
               thread->Ip, opcodeGrp1, opcode, thread->ScriptBufferId);
        if (thread->CallStackDepth) {
          thread->CallStackDepth--;
          uint32_t retBufferId =
              thread->ReturnScriptBufferIds[thread->CallStackDepth];
          thread->Ip = thread->ReturnAdresses[thread->CallStackDepth];
          thread->ScriptBufferId = retBufferId;
        } else {
          ImpLog(LL_Error, LC_VM,
                 "Call stack empty, attempting instruction skip (will most "
                 "likely result in a hang).\n");
          while (*(thread->Ip) != 0xFE) {
            thread->Ip += 1;
          }
        }
      }
    }
#ifndef IMPACTO_DISABLE_IMGUI
    if (DebugThreadId == thread->Id && DebuggerBreak)
      BlockCurrentScriptThread = 1;
#endif
  } while (!BlockCurrentScriptThread);
}

uint8_t* ScriptGetLabelAddress(uint8_t* scriptBufferAdr, uint32_t labelNum) {
  uint8_t* labelTableAdr = (uint8_t*)&scriptBufferAdr[12];
  uint32_t labelAdrRel = SDL_SwapLE32(
      UnalignedRead<uint32_t>(&labelTableAdr[labelNum * sizeof(uint32_t)]));
  return &scriptBufferAdr[labelAdrRel];
}

uint32_t ScriptGetLabelAddressNum(uint8_t* scriptBufferAdr, uint32_t labelNum) {
  uint8_t* labelTableAdr = (uint8_t*)&scriptBufferAdr[12];
  uint32_t labelAdrRel = SDL_SwapLE32(
      UnalignedRead<uint32_t>(&labelTableAdr[labelNum * sizeof(uint32_t)]));
  return labelAdrRel;
}

uint8_t* ScriptGetStrAddress(uint8_t* scriptBufferAdr, uint32_t mesNum) {
  uint32_t stringTableAdrRel =
      SDL_SwapLE32(UnalignedRead<uint32_t>(&scriptBufferAdr[4]));
  uint8_t* stringTableAdr = (uint8_t*)&scriptBufferAdr[stringTableAdrRel];
  uint32_t stringAdrRel = SDL_SwapLE32(
      UnalignedRead<uint32_t>(&stringTableAdr[mesNum * sizeof(uint32_t)]));
  return &scriptBufferAdr[stringAdrRel];
}

uint8_t* ScriptGetTextTableStrAddress(uint32_t textTableId, uint32_t strNum) {
  uint8_t* scriptBufferAdr = TextTable[textTableId].scriptBufferAdr;
  uint32_t stringTableAdrRel =
      SDL_SwapLE32(UnalignedRead<uint32_t>(&scriptBufferAdr[4]));
  uint8_t* stringTableAdr = (uint8_t*)&scriptBufferAdr[stringTableAdrRel];

  uint8_t* textTable = (uint8_t*)TextTable[textTableId].labelAdr;
  uint16_t mesNum =
      UnalignedRead<uint16_t>(&textTable[strNum * sizeof(uint16_t)]);

  uint32_t stringAdrRel = SDL_SwapLE32(
      UnalignedRead<uint32_t>(&stringTableAdr[mesNum * sizeof(uint32_t)]));
  return &scriptBufferAdr[stringAdrRel];
}

uint8_t* ScriptGetRetAddress(uint8_t* scriptBufferAdr, uint32_t retNum) {
  uint32_t returnTableAdrRel =
      SDL_SwapLE32(UnalignedRead<uint32_t>(&scriptBufferAdr[8]));
  uint8_t* returnTableAdr = (uint8_t*)&scriptBufferAdr[returnTableAdrRel];
  uint32_t returnAdrRel = SDL_SwapLE32(
      UnalignedRead<uint32_t>(&returnTableAdr[retNum * sizeof(uint32_t)]));
  return &scriptBufferAdr[returnAdrRel];
}

uint8_t* MsbGetStrAddress(uint8_t* msbBufferAdr, uint32_t mesNum) {
  uint32_t languageCount =
      SDL_SwapLE32(UnalignedRead<uint32_t>(&msbBufferAdr[4]));
  uint32_t stringAreaStartRel =
      SDL_SwapLE32(UnalignedRead<uint32_t>(&msbBufferAdr[12]));
  uint8_t* tableAdrRel = (uint8_t*)&msbBufferAdr[16];

  if (mesNum != 0) {
    while (true) {
      uint32_t id = SDL_SwapLE32(UnalignedRead<uint32_t>(
          &tableAdrRel[(languageCount + 1) * sizeof(uint32_t)]));
      tableAdrRel += (languageCount + 1) * sizeof(uint32_t);
      if (id == mesNum) break;
    }
  }

  uint32_t stringOffset =
      SDL_SwapLE32(UnalignedRead<uint32_t>(&tableAdrRel[1 * sizeof(uint32_t)]));

  return &msbBufferAdr[stringAreaStartRel + stringOffset];
}

}  // namespace Vm
}  // namespace Impacto