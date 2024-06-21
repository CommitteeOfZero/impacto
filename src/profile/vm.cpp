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
  ScrWorkChaStructSize = EnsureGetMemberInt("ScrWorkChaStructSize");
  ScrWorkBgStructSize = EnsureGetMemberInt("ScrWorkBgStructSize");
  TryGetMemberInt("MaxLinkedBgBuffers", MaxLinkedBgBuffers);
  TryGetMemberInt("SystemScriptBuffer", SystemScriptBuffer);
  TryGetMemberInt("SpeakerPortraitsScrWorkOffset",
                  SpeakerPortraitsScrWorkOffset);
  TryGetMemberBool("RestartMaskUsesThreadAlpha", RestartMaskUsesThreadAlpha);

  Pop();
}

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto