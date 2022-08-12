#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../../games/chlcc/systemmenu.h"
#include "../../ui/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {

float BackgroundColor;
Sprite BackgroundFilter;
glm::vec2 CircleStartPosition;
Sprite CircleSprite;
float CircleOffset;
glm::vec2 ErinPosition;
Sprite ErinSprite;
float TitleFadeInDuration;
float TitleFadeOutDuration;
uint32_t FocusTint;
glm::vec2 MenuEntriesPositions[MenuEntriesNum];

static void GetMemberVec2Array(glm::vec2* arr, uint32_t count,
                               char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d vec2 for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementVec2(i);
  }

  Pop();
}

void Configure() {
  BackgroundColor = EnsureGetMemberFloat("BackgroundColor");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");
  CircleStartPosition = EnsureGetMemberVec2("CircleStartPosition");
  CircleSprite = EnsureGetMemberSprite("CircleSprite");
  CircleOffset = EnsureGetMemberFloat("CircleOffset");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  FocusTint = EnsureGetMemberUint("FocusTint");
  GetMemberVec2Array(MenuEntriesPositions, MenuEntriesNum,
                     "MenuEntriesPositions");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SystemMenuPtr = new UI::CHLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto