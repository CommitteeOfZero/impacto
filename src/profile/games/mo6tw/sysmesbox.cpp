#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/mo6tw/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SysMesBox {

void Configure() {
  SelectionHighlight = EnsureGetMember<Sprite>("SelectionHighlight");

  BoxPartLeft = EnsureGetMember<Sprite>("BoxPartLeft");
  BoxPartRight = EnsureGetMember<Sprite>("BoxPartRight");
  BoxPartMiddle = EnsureGetMember<Sprite>("BoxPartMiddle");
  BoxDecoration = EnsureGetMember<Sprite>("BoxDecoration");

  BoxX = EnsureGetMember<float>("BoxX");
  BoxY = EnsureGetMember<float>("BoxY");
  ChoicePadding = EnsureGetMember<float>("ChoicePadding");
  ChoiceY = EnsureGetMember<float>("ChoiceY");
  ChoiceXBase = EnsureGetMember<float>("ChoiceXBase");
  MinMaxMesWidth = EnsureGetMember<float>("MinMaxMesWidth");
  BoxMinimumWidth = EnsureGetMember<float>("BoxMinimumWidth");
  BoxMiddleBaseX = EnsureGetMember<float>("BoxMiddleBaseX");
  BoxMiddleBaseWidth = EnsureGetMember<float>("BoxMiddleBaseWidth");
  BoxMiddleRemainBase = EnsureGetMember<float>("BoxMiddleRemainBase");
  BoxRightBaseX = EnsureGetMember<float>("BoxRightBaseX");
  BoxRightRemainPad = EnsureGetMember<float>("BoxRightRemainPad");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::SysMesBox::DrawType = drawType;
  UI::SysMesBox::Factory = []() -> UI::SysMesBox* {
    return new UI::MO6TW::SysMesBox();
  };
}

}  // namespace SysMesBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto