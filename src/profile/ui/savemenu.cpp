#include "savemenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/savemenu.h"
#include "../games/chlcc/savemenu.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../window.h"
//#include "../../ui/nullmenu.h"

namespace Impacto {
namespace Profile {
namespace SaveMenu {

using namespace Impacto::UI;

SaveMenuType Type = SaveMenuType::None;

Sprite SaveMenuBackgroundSprite;
Sprite EmptyThumbnailSprite;

float EntryStartX;
float EntryXPadding;
float EntryStartY;
float EntryYPadding;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (TryPushMember("SaveMenu")) {
    AssertIs(kObjectType);

    Type = SaveMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    SaveMenuBackgroundSprite =
        EnsureGetMemberSprite("SaveMenuBackgroundSprite");
    EmptyThumbnailSprite = EnsureGetMemberSprite("EmptyThumbnailSprite");

    EntryStartX = EnsureGetMemberFloat("EntryStartX");
    EntryXPadding = EnsureGetMemberFloat("EntryXPadding");
    EntryStartY = EnsureGetMemberFloat("EntryStartY");
    EntryYPadding = EnsureGetMemberFloat("EntryYPadding");

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    if (Type == +SaveMenuType::MO6TW) {
      MO6TW::SaveMenu::Configure();
      CHLCC::SaveMenu::Configure();
    } /* else {
       UI::SaveMenuPtr = new UI::NullMenu();
     }*/


}  // namespace SaveMenu
}  // namespace Profile
}  // namespace Impacto