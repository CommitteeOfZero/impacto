#pragma once

#include "../impacto.h"
#include "../vm/vm.h"

namespace Impacto {
namespace Profile {
namespace Vm {

inline uint32_t StartScript;
inline uint32_t StartScriptBuffer;

inline Impacto::Vm::InstructionSet GameInstructionSet =
    Impacto::Vm::InstructionSet::RNE;

inline bool UseReturnIds = true;
inline bool UseMsbStrings = false;
inline bool UseSeparateMsbArchive = false;
inline bool RestartMaskUsesThreadAlpha = false;

inline int ScrWorkChaStructSize;
inline int ScrWorkBgStructSize;
inline int ScrWorkCaptureStructSize;

inline int MaxLinkedBgBuffers = 1;
inline int SystemScriptBuffer = 1;

inline int SpeakerPortraitsScrWorkOffset = 8;

void Configure();

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto