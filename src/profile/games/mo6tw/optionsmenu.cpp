#include "optionsmenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../ui/optionsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/optionsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace OptionsMenu {

Sprite VoiceToggleEnabledSprites[VoiceToggleCount];
Sprite VoiceToggleDisabledSprites[VoiceToggleCount];
Sprite VoiceToggleHighlightSprite;

glm::vec2 VoiceToggleStart;
glm::vec2 VoiceTogglePadding;
int VoiceTogglePerLine;

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  GetMemberSpriteArray(VoiceToggleEnabledSprites, VoiceToggleCount,
                       "VoiceToggleEnabledSprites");
  GetMemberSpriteArray(VoiceToggleDisabledSprites, VoiceToggleCount,
                       "VoiceToggleDisabledSprites");
  VoiceToggleHighlightSprite =
      EnsureGetMemberSprite("VoiceToggleHighlightSprite");

  VoiceToggleStart = EnsureGetMemberVec2("VoiceToggleStart");
  VoiceTogglePadding = EnsureGetMemberVec2("VoiceTogglePadding");
  VoiceTogglePerLine = EnsureGetMemberInt("VoiceTogglePerLine");

  UI::OptionsMenuPtr = new UI::MO6TW::OptionsMenu();
}

}  // namespace OptionsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
