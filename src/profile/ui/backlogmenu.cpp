#include "backlogmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../window.h"
#include "../../game.h"

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

using namespace Impacto::UI;

Sprite BacklogBackground;
Sprite EntryHighlight;
Sprite VoiceIcon;
Sprite ScrollbarThumb;
Sprite ScrollbarTrack;

float EntryYPadding;
glm::vec2 EntriesStart;
glm::vec2 ScrollbarPosition;
RectF RenderingBounds;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (TryPushMember("BacklogMenu")) {
    AssertIs(kObjectType);

    BacklogBackground = EnsureGetMemberSprite("BacklogBackgroundSprite");
    EntryHighlight = EnsureGetMemberSprite("EntryHighlightSprite");
    VoiceIcon = EnsureGetMemberSprite("VoiceIconSprite");
    ScrollbarThumb = EnsureGetMemberSprite("ScrollbarThumbSprite");
    ScrollbarTrack = EnsureGetMemberSprite("ScrollbarTrackSprite");

    EntryYPadding = EnsureGetMemberFloat("EntryYPadding");
    EntriesStart = EnsureGetMemberVec2("EntriesStart");
    ScrollbarPosition = EnsureGetMemberVec2("ScrollbarPosition");
    RenderingBounds = EnsureGetMemberRectF("RenderingBounds");

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    auto drawType = Game::DrawComponentType::_from_integral_unchecked(
        EnsureGetMemberInt("DrawType"));

    UI::BacklogMenuPtr = new UI::BacklogMenu();
    UI::Menus[drawType].push_back(UI::BacklogMenuPtr);

    Pop();
  }
}

}  // namespace BacklogMenu
}  // namespace Profile
}  // namespace Impacto