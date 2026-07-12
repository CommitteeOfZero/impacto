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
  FadeInDirectDuration = EnsureGetMember<float>("FadeInDirectDuration");
  FadeOutDirectDuration = EnsureGetMember<float>("FadeOutDirectDuration");

  GetMemberArray<glm::vec2>(
      std::span(MenuEntriesPositions, Profile::SystemMenu::MenuEntriesNum),
      "MenuEntriesPositions");

  GetMemberArray<RectF>(
      std::span(MenuEntriesButtonBounds, Profile::SystemMenu::MenuEntriesNum),
      "MenuEntriesButtonBounds");

  SystemMenuBG = EnsureGetMember<Sprite>("SystemMenuBG");
  SystemMenuFrame = EnsureGetMember<Sprite>("SystemMenuFrame");
  MenuButtonGuide = EnsureGetMember<Sprite>("MenuButtonGuide");

  OverlaySprite = EnsureGetMember<Sprite>("OverlaySprite");

  SmokeOpacityNormal = EnsureGetMember<float>("SmokeOpacityNormal");
  SmokeOpacitySystemMenu = EnsureGetMember<float>("SmokeOpacitySystemMenu");
  SmokePosition = EnsureGetMember<glm::vec2>("SmokePosition");
  GetMemberArray<Sprite>(SmokeSprites, "SmokeSprites");
  GetMemberArray<float>(SmokeAnimationDurations, "SmokeAnimationDurations");

  BGDispOffsetTopLeft = EnsureGetMember<glm::vec2>("BGDispOffsetTopLeft");
  BGDispOffsetBottomLeft = EnsureGetMember<glm::vec2>("BGDispOffsetBottomLeft");
  BGDispOffsetTopRight = EnsureGetMember<glm::vec2>("BGDispOffsetTopRight");
  BGDispOffsetBottomRight =
      EnsureGetMember<glm::vec2>("BGDispOffsetBottomRight");
  FrameOffsetTopLeft = EnsureGetMember<glm::vec2>("FrameOffsetTopLeft");
  FrameOffsetBottomLeft = EnsureGetMember<glm::vec2>("FrameOffsetBottomLeft");
  FrameOffsetTopRight = EnsureGetMember<glm::vec2>("FrameOffsetTopRight");
  FrameOffsetBottomRight = EnsureGetMember<glm::vec2>("FrameOffsetBottomRight");

  AngleMultiplier = EnsureGetMember<glm::vec3>("AngleMultiplier");
  BGRandPosRange = EnsureGetMember<glm::vec2>("BGRandPosRange");

  BGTranslationOffset = EnsureGetMember<glm::vec2>("BGTranslationOffset");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::SystemMenuPtr = new UI::CCLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto