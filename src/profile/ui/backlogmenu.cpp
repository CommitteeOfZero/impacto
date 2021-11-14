#include "backlogmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

using namespace Impacto::UI;

Sprite BacklogBackground;
Sprite EntryHighlight;
Sprite VoiceIcon;

float EntryYPadding;
glm::vec2 EntriesStart;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (TryPushMember("BacklogMenu")) {
    AssertIs(kObjectType);

    BacklogBackground = EnsureGetMemberSprite("BacklogBackgroundSprite");
    EntryHighlight = EnsureGetMemberSprite("EntryHighlightSprite");
    VoiceIcon = EnsureGetMemberSprite("VoiceIconSprite");

    EntryYPadding = EnsureGetMemberFloat("EntryYPadding");
    EntriesStart = EnsureGetMemberVec2("EntriesStart");

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    UI::BacklogMenuPtr = new UI::BacklogMenu();

    Pop();
  }
}

}  // namespace BacklogMenu
}  // namespace Profile
}  // namespace Impacto