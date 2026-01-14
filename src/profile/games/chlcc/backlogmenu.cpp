#include "backlogmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/backlogmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace BacklogMenu {

void Configure() {
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");
  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPosition");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPosition");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");

  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("BacklogButtonPromptSprite");
  BacklogBackgroundSprite = EnsureGetMember<Sprite>("BacklogBackgroundSprite");
  EntryHighlightSprite = EnsureGetMember<Sprite>("EntryHighlightSprite");
  VoiceIconSprite = EnsureGetMember<Sprite>("VoiceIconSprite");
  ScrollbarThumbSprite = EnsureGetMember<Sprite>("ScrollbarThumbSprite");
  ScrollbarPosition = EnsureGetMember<glm::vec2>("ScrollbarPosition");
  EntriesStart = EnsureGetMember<glm::vec2>("EntriesStart");
  RenderingBounds = EnsureGetMember<RectF>("RenderingBounds");
  EntryYPadding = EnsureGetMember<float>("EntryYPadding");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::BacklogMenuPtr = new UI::CHLCC::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto