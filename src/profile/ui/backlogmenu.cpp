#include "backlogmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../game.h"

#include "../../ui/widgets/backlogentry.h"

#include "../games/mo6tw/backlogmenu.h"
#include "../games/chlcc/backlogmenu.h"
#include "../games/cc/backlogmenu.h"

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("BacklogMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        BacklogMenuType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    if (Type == +BacklogMenuType::None) {
      UI::BacklogMenuPtr = new UI::BacklogMenu();
      UI::Menus[Game::DrawComponentType::None].push_back(UI::BacklogMenuPtr);

      Pop();
      return;
    }

    EntryHighlightLocation =
        EntryHighlightLocationType::_from_integral_unchecked(
            EnsureGetMember<int>("EntryHighlightLocation"));

    BacklogBackground = EnsureGetMember<Sprite>("BacklogBackgroundSprite");
    EntryHighlight = EnsureGetMember<Sprite>("EntryHighlightSprite");
    VoiceIcon = EnsureGetMember<Sprite>("VoiceIconSprite");
    ScrollbarThumb = EnsureGetMember<Sprite>("ScrollbarThumbSprite");
    ScrollbarTrack = EnsureGetMember<Sprite>("ScrollbarTrackSprite");

    EntryYPadding = EnsureGetMember<float>("EntryYPadding");
    EntriesStart = EnsureGetMember<glm::vec2>("EntriesStart");
    EntryHighlightOffset = EnsureGetMember<glm::vec2>("EntryHighlightOffset");
    EntryHighlightPadding = EnsureGetMember<float>("EntryHighlightPadding");
    VoiceIconOffset = EnsureGetMember<glm::vec2>("VoiceIconOffset");
    ScrollbarPosition = EnsureGetMember<glm::vec2>("ScrollbarPosition");
    ScrollbarThumbLength = EnsureGetMember<float>("ScrollbarThumbLength");
    RenderingBounds = EnsureGetMember<RectF>("RenderingBounds");
    HoverBounds = EnsureGetMember<RectF>("HoverBounds");

    FadeInDuration = EnsureGetMember<float>("FadeInDuration");
    FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

    ScrollingSpeed = EnsureGetMember<float>("ScrollingSpeed");
    PageUpDownHeight = EnsureGetMember<float>("PageUpDownHeight");

    switch (Type) {
      case BacklogMenuType::MO6TW:
        MO6TW::BacklogMenu::Configure();
        break;

      case BacklogMenuType::CHLCC:
        CHLCC::BacklogMenu::Configure();
        break;

      case BacklogMenuType::CC:
        CC::BacklogMenu::Configure();
        break;

      case BacklogMenuType::None:
        break;
    }

    Pop();
  }
}

}  // namespace BacklogMenu
}  // namespace Profile
}  // namespace Impacto