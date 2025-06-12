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
  Background = EnsureGetMember<Sprite>("SystemMenuBackground");
  BackgroundPosition =
      EnsureGetMember<glm::vec2>("SystemMenuBackgroundPosition");

  MenuItemsLine = EnsureGetMember<Sprite>("SystemMenuItemsLine");
  MenuItemsLinePosition =
      EnsureGetMember<glm::vec2>("SystemMenuItemsLinePosition");
  MainMenuTitleText = EnsureGetMember<Sprite>("MainMenuTitleText");
  MainMenuLabelRightPosition =
      EnsureGetMember<glm::vec2>("MainMenuLabelRightPosition");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  MenuTitleTextPosition = EnsureGetMember<glm::vec2>("MenuTitleTextPosition");
  MenuSelectionDot = EnsureGetMember<Sprite>("SystemMenuSelectionDot");
  MenuSelectionDotPosition =
      EnsureGetMember<glm::vec2>("SystemMenuSelectionDotPosition");
  MenuSelectionDotMultiplier =
      EnsureGetMember<float>("SystemMenuSelectionDotMultiplier");
  MenuSelection = EnsureGetMember<Sprite>("SystemMenuSelection");
  MenuSelectionPosition =
      EnsureGetMember<glm::vec2>("SystemMenuSelectionPosition");
  SelectMenuHeaderCount = EnsureGetMember<int>("SelectMenuSpritesCount");
  GetMemberArray<Sprite>(SelectMenuHeader, SelectMenuHeaderCount,
                         "SelectMenuSprites");
  GetMemberArray<glm::vec2>(SelectMenuHeaderPositions, SelectMenuHeaderCount,
                            "SelectMenuTextPositions");
  MenuLoopDuration = EnsureGetMember<float>("MenuLoopDuration");
  HoverLerpSpeed = EnsureGetMember<float>("MenuHoverLerpSpeed");
  MenuRunningSelectedLabel =
      EnsureGetMember<Sprite>("SystemMenuRunningSelectedLabel");
  MenuRunningSelectedLabelPosition =
      EnsureGetMember<glm::vec2>("SystemMenuRunningSelectedLabelPosition");
  MenuRunningSelectedLabelAngle =
      EnsureGetMember<float>("SystemMenuRunningSelectedLabelAngle");
  MenuButtonPrompt = EnsureGetMember<Sprite>("SystemMenuButtonPrompt");
  MenuButtonPromptPosition =
      EnsureGetMember<glm::vec2>("SystemMenuButtonPromptPosition");
  RedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  SelectedLabelSpeed = EnsureGetMember<float>("MenuSelectedLabelSpeed");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::SystemMenuPtr = new UI::CHLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto