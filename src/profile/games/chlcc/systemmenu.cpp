#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"
#include "../../../log.h"

#include "../../../games/chlcc/systemmenu.h"
#include "../../ui/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {

void Configure() {
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CircleOffset = EnsureGetMember<float>("CircleOffset");
  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  FocusTint = EnsureGetMember<uint32_t>("FocusTint");
  GetMemberArray<glm::vec2>(MenuEntriesPositions,
                            Profile::SystemMenu::MenuEntriesNum,
                            "MenuEntriesPositions");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SystemMenuPtr = new UI::CHLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto