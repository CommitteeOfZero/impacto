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

inline int ScrWorkChaStructSize = 0;
inline int ScrWorkChaOffsetStructSize = 0;
inline int ScrWorkBgStructSize = 0;
inline int ScrWorkBgOffsetStructSize = 0;
inline int ScrWorkCaptureStructSize = 0;
inline int ScrWorkCaptureOffsetStructSize = 0;
inline int ScrWorkCaptureEffectInfoStructSize = 0;
inline int ScrWorkBgEffStructSize = 0;
inline int ScrWorkBgEffOffsetStructSize = 0;

inline int MaxLinkedBgBuffers = 1;
inline int SystemScriptBuffer = 1;

inline int SpeakerPortraitsScrWorkOffset = 8;

void Configure();

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto