#include "clearlistmenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/clearlistmenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace ClearListMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  GetMemberArray<Sprite>(std::span(TubeSprites, EndingCount), "TubeSprites");
  GetMemberArray<glm::vec2>(std::span(TubePositions, EndingCount),
                            "TubePositions");
  GetMemberArray<Sprite>(std::span(LabelSprites, EndingCount), "LabelSprites");
  GetMemberArray<glm::vec2>(std::span(LabelPositions, EndingCount),
                            "LabelPositions");
  GetMemberArray<int>(std::span(EndingFlags, EndingCount), "EndingFlags");
  GuideSprite = EnsureGetMember<Sprite>("GuideSprite");
  GuidePos = EnsureGetMember<glm::vec2>("GuidePos");
  CountMax = EnsureGetMember<int>("CountMax");

  UI::ClearListMenuPtr = new UI::SG0::ClearListMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::ClearListMenuPtr);
}

}  // namespace ClearListMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
