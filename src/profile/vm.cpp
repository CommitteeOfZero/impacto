#include "vm.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Vm {

void Configure() {
  EnsurePushMemberOfType("Vm", LUA_TTABLE);

  StartScript = EnsureGetMemberUint("StartScript");
  StartScriptBuffer = EnsureGetMemberUint("StartScriptBuffer");

  GameInstructionSet = Impacto::Vm::InstructionSet::_from_integral_unchecked(
      EnsureGetMemberInt("GameInstructionSet"));

  UseReturnIds = EnsureGetMemberBool("UseReturnIds");
  TryGetMemberBool("UseMsbStrings", UseMsbStrings);
  TryGetMemberBool("UseSeparateMsbArchive", UseSeparateMsbArchive);
  TryGetMemberBool("RestartMaskUsesThreadAlpha", RestartMaskUsesThreadAlpha);

  ScrWorkChaStructSize = EnsureGetMemberInt("ScrWorkChaStructSize");
  ScrWorkBgStructSize = EnsureGetMemberInt("ScrWorkBgStructSize");
  ScrWorkCaptureStructSize = EnsureGetMemberInt("ScrWorkCaptureStructSize");

  TryGetMemberInt("MaxLinkedBgBuffers", MaxLinkedBgBuffers);
  TryGetMemberInt("SystemScriptBuffer", SystemScriptBuffer);

  TryGetMemberInt("SpeakerPortraitsScrWorkOffset",
                  SpeakerPortraitsScrWorkOffset);

  Pop();
}

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto