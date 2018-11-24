#include "charset.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Charset {

uint8_t* Flags = 0;

void Load() {
  auto const& _flags = EnsureGetMemberOfType(
      EnsureGetMemberOfType(Json, "/", "Charset", kObjectType), "/Charset",
      "Flags", kArrayType);

  Flags = (uint8_t*)malloc(_flags.Size());
  int i = 0;
  for (auto it = _flags.Begin(); it != _flags.End(); it++) {
    Flags[i] = EnsureGetInt(*it, "/Charset/Flags/x");
    i++;
  }
}

}  // namespace Charset
}  // namespace Profile
}  // namespace Impacto