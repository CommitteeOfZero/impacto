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
  auto const& _vm = EnsureGetMemberOfType(Json, "/", "Vm", kObjectType);

  StartScript = EnsureGetMemberUint(_vm, "/Vm", "StartScript");
  StartScriptBuffer = EnsureGetMemberUint(_vm, "/Vm", "StartScriptBuffer");
  GameInstructionSet = Impacto::Vm::InstructionSet::_from_integral_unchecked(
      EnsureGetMemberInt(_vm, "/Vm", "GameInstructionSet"));
}

}  // namespace Vm
}  // namespace Profile
}  // namespace Impacto