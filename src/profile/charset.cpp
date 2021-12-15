#include "charset.h"
#include "profile_internal.h"

#include "../../vendor/utf8-cpp/utf8.h"

namespace Impacto {
namespace Profile {
namespace Charset {

uint8_t* Flags = 0;
ska::flat_hash_map<uint32_t, uint16_t> CharacterToSc3;

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

  if (TryPushMember("CharacterToSc3")) {
    AssertIs(kObjectType);

    auto const& _textToSc3Hash = TopVal();
    for (Value::ConstMemberIterator it = _textToSc3Hash.MemberBegin();
         it != _textToSc3Hash.MemberEnd(); it++) {
      std::string key(EnsureGetKeyString(it));

      std::string::iterator strIt = key.begin();
      std::string::iterator strEnd = key.end();
      auto codePoint = utf8::next(strIt, strEnd);

      EnsurePushMemberIteratorOfType(it, kNumberType);

      CharacterToSc3[codePoint] = EnsureGetUint();

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Charset
}  // namespace Profile
}  // namespace Impacto