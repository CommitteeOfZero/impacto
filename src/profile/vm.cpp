#include "vm.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Vm {

uint32_t StartScript;
uint32_t StartScriptBuffer;
Impacto::Vm::InstructionSet GameInstructionSet =
    Impacto::Vm::InstructionSet::RNE;
bool UseReturnIds = true;
bool UseMsbStrings = false;
int ScrWorkChaStructSize;
int ScrWorkBgStructSize;
int MaxLinkedBgBuffers = 2;

void Configure() {
  EnsurePushMemberOfType("Vm", kObjectType);

  StartScript = EnsureGetMemberUint("StartScript");
  StartScriptBuffer = EnsureGetMemberUint("StartScriptBuffer");
  GameInstructionSet = Impacto::Vm::InstructionSet::_from_integral_unchecked(
      EnsureGetMemberInt("GameInstructionSet"));
  UseReturnIds = EnsureGetMemberBool("UseReturnIds");
  TryGetMemberBool("UseMsbStrings", UseMsbStrings);
  ScrWorkChaStructSize = EnsureGetMemberInt("ScrWorkChaStructSize");
  ScrWorkBgStructSize = EnsureGetMemberInt("ScrWorkBgStructSize");
  TryGetMemberInt("MaxLinkedBgBuffers", MaxLinkedBgBuffers);

  Pop();
}

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto