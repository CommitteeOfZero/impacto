#include "charset.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Charset {

uint8_t* Flags = 0;

void Load() {
  EnsurePushMemberOfType("Charset", kObjectType);
  {
    EnsurePushMemberOfType("Flags", kArrayType);

    auto const& _flags = TopVal();
    uint32_t flagsCount = _flags.Size();
    Flags = (uint8_t*)malloc(flagsCount);
    for (uint32_t i = 0; i < flagsCount; i++) {
      Flags[i] = EnsureGetArrayElementInt(i);
    }

    Pop();
  }
  Pop();
}

}  // namespace Charset
}  // namespace Profile
}  // namespace Impacto