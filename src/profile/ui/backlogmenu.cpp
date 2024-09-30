#include "backlogmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../game.h"

#include "../../ui/widgets/backlogentry.h"

#include "../games/mo6tw/backlogmenu.h"
#include "../games/cc/backlogmenu.h"

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("BacklogMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        BacklogMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +BacklogMenuType::None) {
      UI::BacklogMenuPtr = new UI::BacklogMenu();
      UI::Menus[Game::DrawComponentType::None].push_back(UI::BacklogMenuPtr);

      Pop();
      return;
    }

    EntryHighlightLocation =
        EntryHighlightLocationType::_from_integral_unchecked(
            EnsureGetMemberInt("EntryHighlightLocation"));

    BacklogBackground = EnsureGetMemberSprite("BacklogBackgroundSprite");
    EntryHighlight = EnsureGetMemberSprite("EntryHighlightSprite");
    VoiceIcon = EnsureGetMemberSprite("VoiceIconSprite");
    ScrollbarThumb = EnsureGetMemberSprite("ScrollbarThumbSprite");
    ScrollbarTrack = EnsureGetMemberSprite("ScrollbarTrackSprite");

    EntryYPadding = EnsureGetMemberFloat("EntryYPadding");
    EntriesStart = EnsureGetMemberVec2("EntriesStart");
    EntryHighlightOffset = EnsureGetMemberVec2("EntryHighlightOffset");
    VoiceIconOffset = EnsureGetMemberVec2("VoiceIconOffset");
    ScrollbarPosition = EnsureGetMemberVec2("ScrollbarPosition");
    ScrollbarThumbLength = EnsureGetMemberFloat("ScrollbarThumbLength");
    RenderingBounds = EnsureGetMemberRectF("RenderingBounds");
    HoverBounds = EnsureGetMemberRectF("HoverBounds");

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    ScrollingSpeed = EnsureGetMemberFloat("ScrollingSpeed");
    MinHoldTime = EnsureGetMemberFloat("MinHoldTime");
    AdvanceFocusTimeInterval = EnsureGetMemberFloat("AdvanceFocusTimeInterval");
    PageUpDownHeight = EnsureGetMemberFloat("PageUpDownHeight");

    switch (Type) {
      case BacklogMenuType::MO6TW:
        MO6TW::BacklogMenu::Configure();
        break;
      case BacklogMenuType::CC:
        CC::BacklogMenu::Configure();
        break;
    }

    Pop();
  }
}

}  // namespace BacklogMenu
}  // namespace Profile
}  // namespace Impacto