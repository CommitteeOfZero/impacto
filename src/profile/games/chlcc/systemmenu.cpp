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
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  FocusTint = EnsureGetMember<uint32_t>("FocusTint");
  GetMemberArray<glm::vec2>(
      std::span(MenuEntriesPositions, Profile::SystemMenu::MenuEntriesNum),
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

  SelectMenuHeader = EnsureGetMember<std::vector<Sprite>>("SelectMenuSprites");
  SelectMenuHeaderPositions =
      EnsureGetMember<std::vector<glm::vec2>>("SelectMenuTextPositions");
  if (SelectMenuHeader.size() != SelectMenuHeaderPositions.size()) {
    throw std::runtime_error("Related arrays have mismatching sizes");
  }

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
  SelectedLabelSpeed = EnsureGetMember<float>("MenuSelectedLabelSpeed");
  StarAnimationDuration = EnsureGetMember<float>("StarAnimationDuration");
  StarRotationSpeed = EnsureGetMember<float>("StarRotationSpeed");
  LeftAngle = EnsureGetMember<float>("LeftAngle");
  GetMemberArray<glm::vec2>(std::span(StarsOffsetsStart.data(), STAR_COUNT),
                            "StarsOffsetsStart");
  GetMemberArray<glm::vec2>(std::span(StarsOffsetsEnd.data(), STAR_COUNT),
                            "StarsOffsetsEnd");
  StarSprite = EnsureGetMember<Sprite>("StarSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::SystemMenuPtr = new UI::CHLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto