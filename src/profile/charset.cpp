#include "charset.h"
#include "profile_internal.h"
#include "../text.h"

#include <utf8.h>

namespace Impacto {
namespace Profile {
namespace Charset {

void Load() {
  EnsurePushMemberOfType("Charset", LUA_TTABLE);

  if (TryPushMember("Flags")) {
    AssertIs(LUA_TTABLE);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      uint16_t glyphId = EnsureGetKey<uint16_t>();
      const uint8_t flags = EnsureGetArrayElement<uint8_t>();

      StringToken::AddFlags(glyphId, flags);

      Pop();
    }

    Pop();
  }

  if (TryPushMember("CharacterToSc3")) {
    AssertIs(LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      std::string key(EnsureGetKey<std::string>());

      std::string::iterator strIt = key.begin();
      std::string::iterator strEnd = key.end();
      auto codePoint = utf8::next(strIt, strEnd);

      CharacterToSc3[codePoint] = EnsureGetArrayElement<uint16_t>();

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Charset
}  // namespace Profile
}  // namespace Impacto
