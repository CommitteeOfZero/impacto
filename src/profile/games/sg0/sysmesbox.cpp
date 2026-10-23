#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace SysMesBox {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  BackgroundOpacity = EnsureGetMember<float>("BackgroundOpacity");
  MessageColor = EnsureGetMember<uint32_t>("MessageColor");
  ChoiceColor = EnsureGetMember<uint32_t>("ChoiceColor");
  ChoiceHighlightColor = EnsureGetMember<uint32_t>("ChoiceHighlightColor");
  OutlineColor = EnsureGetMember<uint32_t>("OutlineColor");
  ChoiceSpacing = EnsureGetMember<float>("ChoiceSpacing");
  ChoicePadding = EnsureGetMember<glm::vec2>("ChoicePadding");

  UI::SysMesBox::DrawType =
      EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::SysMesBox::Factory = []() -> UI::SysMesBox* {
    return new UI::SG0::SysMesBox();
  };
}

}  // namespace SysMesBox
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
