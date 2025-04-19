#include "vm.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Vm {

void Configure() {
  EnsurePushMemberOfType("Vm", LUA_TTABLE);

  StartScript = EnsureGetMember<uint32_t>("StartScript");
  StartScriptBuffer = EnsureGetMember<uint32_t>("StartScriptBuffer");

  GameInstructionSet = Impacto::Vm::InstructionSet::_from_integral_unchecked(
      EnsureGetMember<int>("GameInstructionSet"));

  UseReturnIds = EnsureGetMember<bool>("UseReturnIds");
  TryGetMember<bool>("UseMsbStrings", UseMsbStrings);
  TryGetMember<bool>("UseSeparateMsbArchive", UseSeparateMsbArchive);
  TryGetMember<bool>("RestartMaskUsesThreadAlpha", RestartMaskUsesThreadAlpha);

  ScrWorkChaStructSize = EnsureGetMember<int>("ScrWorkChaStructSize");
  ScrWorkChaOffsetStructSize =
      EnsureGetMember<int>("ScrWorkChaOffsetStructSize");
  ScrWorkBgStructSize = EnsureGetMember<int>("ScrWorkBgStructSize");
  ScrWorkBgOffsetStructSize = EnsureGetMember<int>("ScrWorkBgOffsetStructSize");
  ScrWorkCaptureStructSize = EnsureGetMember<int>("ScrWorkCaptureStructSize");
  ScrWorkCaptureOffsetStructSize =
      EnsureGetMember<int>("ScrWorkCaptureStructSize");
  ScrWorkBgEffStructSize = EnsureGetMember<int>("ScrWorkBgEffStructSize");
  ScrWorkBgEffOffsetStructSize =
      EnsureGetMember<int>("ScrWorkBgEffOffsetStructSize");

  TryGetMember<int>("MaxLinkedBgBuffers", MaxLinkedBgBuffers);
  TryGetMember<int>("SystemScriptBuffer", SystemScriptBuffer);

  TryGetMember<int>("SpeakerPortraitsScrWorkOffset",
                    SpeakerPortraitsScrWorkOffset);

  Pop();
}

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto