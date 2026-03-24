#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/clearlistmenu.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace ClearListMenu {

void Configure() {
  ClearListBookLayerSprite =
      EnsureGetMember<Sprite>("ClearListBookLayerSprite");
  ClearListGuideSprite = EnsureGetMember<Sprite>("ClearListGuideSprite");
  ClearListMaskSprite = EnsureGetMember<Sprite>("ClearListMaskSprite");

  GetMemberArray<Sprite>(std::span(EndingSprites, Endings), "EndingSprites");
  EndingSpriteOffsetY = EnsureGetMember<float>("EndingSpriteOffsetY");
  MenuOffsetY = EnsureGetMember<float>("MenuOffsetY");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  ClearListGuidePosition = EnsureGetMember<glm::vec2>("ClearListGuidePosition");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  auto clearList = new UI::CCLCC::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto