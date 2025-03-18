#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"
#include "../../../log.h"

#include "../../../games/cclcc/systemmenu.h"
#include "../../ui/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SystemMenu {

void Configure() {
  MoveInDuration = EnsureGetMember<float>("MoveInDuration");
  MoveOutDuration = EnsureGetMember<float>("MoveOutDuration");
  ItemsFadeInDuration = EnsureGetMember<float>("ItemsFadeInDuration");
  ItemsFadeOutDuration = EnsureGetMember<float>("ItemsFadeOutDuration");

  GetMemberArray<glm::vec2>(MenuEntriesPositions,
                            Profile::SystemMenu::MenuEntriesNum,
                            "MenuEntriesPositions");

  GetMemberArray<RectF>(MenuEntriesButtonBounds,
                        Profile::SystemMenu::MenuEntriesNum,
                        "MenuEntriesButtonBounds");

  SystemMenuBG = EnsureGetMember<Sprite>("SystemMenuBG");
  SystemMenuFrame = EnsureGetMember<Sprite>("SystemMenuFrame");
  MenuButtonGuide = EnsureGetMember<Sprite>("MenuButtonGuide");
  SystemMenuMask = EnsureGetMember<Sprite>("SystemMenuMask");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SystemMenuPtr = new UI::CCLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto