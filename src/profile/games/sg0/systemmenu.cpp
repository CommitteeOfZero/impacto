#include "systemmenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace SystemMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  MonitorScreen = EnsureGetMember<RectF>("MonitorScreen");
  GlassSprite = EnsureGetMember<Sprite>("GlassSprite");
  GlassOpacity = EnsureGetMember<float>("GlassOpacity");
  RingSprite = EnsureGetMember<Sprite>("RingSprite");
  RingPos = EnsureGetMember<glm::vec2>("RingPos");
  GetMemberArray<glm::vec2>(
      std::span(EntryCenters, Profile::SystemMenu::MenuEntriesNum),
      "EntryCenters");
  NavOrder = EnsureGetMember<std::vector<int>>("NavOrder");
  FocusScale = EnsureGetMember<float>("FocusScale");
  UnfocusedBrightness = EnsureGetMember<float>("UnfocusedBrightness");
  CountMax = EnsureGetMember<int>("CountMax");

  UI::SystemMenuPtr = new UI::SG0::SystemMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
