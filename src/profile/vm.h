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
extern int32_t ScrWorkChaStructSize;
extern int32_t ScrWorkBgStructSize;
extern int32_t MaxLinkedBgBuffers;
extern int32_t SystemScriptBuffer;
extern int32_t SpeakerPortraitsScrWorkOffset;

void Configure();

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto