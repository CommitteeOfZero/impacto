#include "vm.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Vm {

uint32_t StartScript;
uint32_t StartScriptBuffer;
Impacto::Vm::InstructionSet GameInstructionSet =
    Impacto::Vm::InstructionSet::RNE;

void Configure() {
  EnsurePushMemberOfType("Vm", kObjectType);

  StartScript = EnsureGetMemberUint("StartScript");
  StartScriptBuffer = EnsureGetMemberUint("StartScriptBuffer");
  GameInstructionSet = Impacto::Vm::InstructionSet::_from_integral_unchecked(
      EnsureGetMemberInt("GameInstructionSet"));

  Pop();
}

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto