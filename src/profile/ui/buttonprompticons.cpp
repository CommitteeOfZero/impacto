#include "buttonprompticons.h"
#include "../profile_internal.h"
#include "../patch.h"

namespace Impacto {
namespace Profile {
namespace ButtonPromptIcons {

void Configure() {
  if (!Patch::UniversalButtonIcons) return;

  EnsurePushMemberOfType("ButtonPromptIcons", LUA_TTABLE);
  ControllerIcons = TryGetMember<decltype(ControllerIcons)>("Controller")
                        .value_or(decltype(ControllerIcons){});
  KeyboardIcons = TryGetMember<decltype(KeyboardIcons)>("Keyboard")
                      .value_or(decltype(KeyboardIcons){});
  Pop();
}

}  // namespace ButtonPromptIcons
}  // namespace Profile
}  // namespace Impacto
