#include "titlemenu.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace TitleMenu {

void Configure() {
  LogoSprite = EnsureGetMember<Sprite>("LogoSprite");
  LogoPos = EnsureGetMember<glm::vec2>("LogoPos");

  ItemPos = EnsureGetMember<glm::vec2>("ItemPos");
  ItemSpacing = EnsureGetMember<float>("ItemSpacing");
  GetMemberArray<Sprite>(std::span(SubItemSprites, SubItemsNum),
                         "SubItemSprites");
  GetMemberArray<Sprite>(std::span(SubItemHSprites, SubItemsNum),
                         "SubItemHighlightedSprites");
  SubItemOffsetX = EnsureGetMember<float>("SubItemOffsetX");
  SubItemSpacing = EnsureGetMember<float>("SubItemSpacing");

  PressToStartAnimFastDurationIn =
      EnsureGetMember<float>("PressToStartAnimFastDurationIn");
  PressToStartAnimFastDurationOut =
      EnsureGetMember<float>("PressToStartAnimFastDurationOut");

  UI::SG0::TitleMenu* menu = new UI::SG0::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  UI::TitleMenuPtr = menu;

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
