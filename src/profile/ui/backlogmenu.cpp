#include "backlogmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../game.h"

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("BacklogMenu")) {
    AssertIs(LUA_TTABLE);

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