#include "charset.h"
#include "profile_internal.h"

#include <utf8-cpp/source/utf8.h>

namespace Impacto {
namespace Profile {
namespace Charset {

void Load() {
  EnsurePushMemberOfType("Charset", LUA_TTABLE);
  {
    EnsurePushMemberOfType("Flags", LUA_TTABLE);

    uint32_t flagsCount = (uint32_t)lua_rawlen(LuaState, -1);
    Flags.resize(flagsCount + 1);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKey<int32_t>();
      Flags[i] = EnsureGetArrayElement<int>();
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

      uint32_t test = EnsureGetArrayElement<uint32_t>();
      CharacterToSc3[codePoint] = test;

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Charset
}  // namespace Profile
}  // namespace Impacto
