#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsMenu {

float MenuTransitionDuration;
uint32_t BackgroundColor;
Sprite CircleSprite;
glm::vec2 CircleStartPosition;
float CircleOffset;
glm::vec2 ErinPosition;
Sprite ErinSprite;
Sprite BackgroundFilter;
glm::vec2 InitialRedBarPosition;
glm::vec2 RightRedBarPosition;
Sprite InitialRedBarSprite;
glm::vec2 RedBarPosition;
float RedBarDivision;
float RedBarBaseX;
Sprite RedBarSprite;
glm::vec2 RedBarLabelPosition;
Sprite RedBarLabel;
glm::vec2 MenuTitleTextRightPosition;
glm::vec2 MenuTitleTextLeftPosition;
float MenuTitleTextAngle;
float TitleFadeInDuration;
float TitleFadeOutDuration;
Sprite MenuTitleText;

static void GetMemberVec2Array(glm::vec2* arr, uint32_t count,
                               char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d vec2 for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementVec2(i);
  }

  Pop();
}

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  MenuTransitionDuration = EnsureGetMemberFloat("TransitionDuration");
  BackgroundColor = EnsureGetMemberUint("BackgroundColor");
  CircleSprite = EnsureGetMemberSprite("CircleSprite");
  CircleStartPosition = EnsureGetMemberVec2("CircleStartPosition");
  CircleOffset = EnsureGetMemberFloat("CircleOffset");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMemberVec2("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMemberVec2("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMemberSprite("RedBarSprite");
  RedBarDivision = EnsureGetMemberFloat("RedBarDivision");
  RedBarBaseX = EnsureGetMemberFloat("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMemberVec2("RedBarLabelPosition");
  RedBarLabel = EnsureGetMemberSprite("RedBarLabel");
  MenuTitleTextRightPosition = EnsureGetMemberVec2("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition = EnsureGetMemberVec2("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMemberFloat("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMemberSprite("MenuTitleText");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TipsMenuPtr = new UI::CHLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
