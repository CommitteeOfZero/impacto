#pragma once

#include "../impacto.h"
#include "../vm/vm.h"

namespace Impacto {
namespace Profile {
namespace Vm {

extern uint32_t StartScript;
extern uint32_t StartScriptBuffer;
extern Impacto::Vm::InstructionSet GameInstructionSet;
extern bool UseReturnIds;
extern bool UseMsbStrings;
extern bool UseSeparateMsbArchive;
extern int ScrWorkChaStructSize;
extern int ScrWorkBgStructSize;
extern int MaxLinkedBgBuffers;
extern int SystemScriptBuffer;

void Configure();

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto