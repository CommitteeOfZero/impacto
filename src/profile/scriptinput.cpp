#include "scriptinput.h"
#include "profile_internal.h"

#include "../vm/interface/input.h"

namespace Impacto {
namespace Profile {

template <>
struct TryGetImpl<ScriptInput::KeyboardPadMapping> {
  static std::optional<ScriptInput::KeyboardPadMapping> Call() {
    using ScriptInput::KeyboardPadMapping;
    if (!lua_istable(LuaState, -1)) return std::nullopt;
    KeyboardPadMapping kbPadMap;
    uint8_t kbMode{};
    if (TryGetMember("Id", kbPadMap.Id) && TryGetMember("Mode", kbMode)) {
      kbPadMap.Mode = static_cast<KeyboardPadMapping::InputMode>(kbMode);
      return kbPadMap;
    }
    return std::nullopt;
  }
};

template <>
struct TryGetImpl<ScriptInput::InputAxisDir> {
  static std::optional<ScriptInput::InputAxisDir> Call() {
    using UnderlyingT = std::underlying_type_t<ScriptInput::InputAxisDir>;
    std::optional<UnderlyingT> opt = TryGet<UnderlyingT>();
    if (opt) return static_cast<ScriptInput::InputAxisDir>(*opt);
    return std::nullopt;
  }
};

namespace ScriptInput {
using namespace Impacto::Vm::Interface;

void Configure() {
  EnsurePushMemberOfType("Input", LUA_TTABLE);

  PADToKBcustom = EnsureGetMember<decltype(PADToKBcustom)>("PADtoKBcustom");
  PADToMouse = EnsureGetMember<decltype(PADToMouse)>("PADtoMS");
  PADToController = EnsureGetMember<decltype(PADToController)>("PADtoGP");
  PADToControllerAxis =
      EnsureGetMember<decltype(PADToControllerAxis)>("PADtoGPA");
  PADcustomA = EnsureGetMember<std::vector<uint32_t>>("PADcustomA");
  PADcustomB = EnsureGetMember<std::vector<uint32_t>>("PADcustomB");

  KBcustom = EnsureGetMember<decltype(KBcustom)>("KBcustom");

  Pop();
}

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto