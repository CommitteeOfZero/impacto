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
  MainMenuLabel = EnsureGetMember<Sprite>("MainMenuLabel");
  MainMenuLabelRightPosition = EnsureGetMember<glm::vec2>("MainMenuLabelRightPosition");
  MainMenuLabelRightAngle =
      EnsureGetMember<float>("MainMenuLabelRightAngle");
  MainMenuLabelPosition = EnsureGetMember<glm::vec2>("MainMenuLabelPosition");
  MenuSelectionDot = EnsureGetMember<Sprite>("SystemMenuSelectionDot");
  MenuSelectionDotPosition =
      EnsureGetMember<glm::vec2>("SystemMenuSelectionDotPosition");
  MenuSelectionDotMultiplier =
      EnsureGetMember<float>("SystemMenuSelectionDotMultiplier");
  MenuSelection = EnsureGetMember<Sprite>("SystemMenuSelection");
    MenuSelectionPosition =
        EnsureGetMember<glm::vec2>("SystemMenuSelectionPosition");
  SelectMenuHeader = EnsureGetMember<Sprite>("SystemSelectMenuHeader");
  SelectMenuHeaderPosition =
      EnsureGetMember<glm::vec2>("SystemSelectMenuHeaderPosition");
  MenuRunningSelectedLabel =
      EnsureGetMember<Sprite>("SystemMenuRunningSelectedLabel");
  MenuButtonPrompt = EnsureGetMember<Sprite>("SystemMenuButtonPrompt");
  MenuButtonPromptPosition =
      EnsureGetMember<glm::vec2>("SystemMenuButtonPromptPosition");
  MenuLine = EnsureGetMember<Sprite>("SystemMenuLine");
  MenuLinePosition = EnsureGetMember<glm::vec2>("SystemMenuLinePosition");
  MenuCHLCCLabel = EnsureGetMember<Sprite>("SystemMenuCHLCCLabel");
  MenuCHLCCLabelPosition =
      EnsureGetMember<glm::vec2>("SystemMenuCHLCCLabelPosition");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SystemMenuPtr = new UI::CHLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto