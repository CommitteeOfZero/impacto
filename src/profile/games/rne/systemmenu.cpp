#include "systemmenu.h"
#include "../../profile_internal.h"
#include "tilebackground.h"

#include "../../ui/systemmenu.h"
#include "../../../ui/ui.h"
#include "../../../games/rne/systemmenu.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace SystemMenu {

void Configure() {
  EnsurePushMemberOfType("Background", LUA_TTABLE);
  BackgroundAnimation = RNE::ParseTileBackground();
  RNE::ParseTileBackground();
  Pop();

  SkyBackgroundSprite = EnsureGetMember<Sprite>("SkyBackgroundSprite");
  SkyArrowSprite = EnsureGetMember<Sprite>("SkyArrowSprite");
  SkyTextSprite = EnsureGetMember<Sprite>("SkyTextSprite");
  SkyBackgroundBeginX = EnsureGetMember<float>("SkyBackgroundBeginX");
  SkyBackgroundY = EnsureGetMember<float>("SkyBackgroundY");
  SkyTextBeginX = EnsureGetMember<float>("SkyTextBeginX");
  SkyTextY = EnsureGetMember<float>("SkyTextY");
  MenuEntriesXSkew = EnsureGetMember<float>("MenuEntriesXSkew");
  MenuEntriesTargetWidth = EnsureGetMember<float>("MenuEntriesTargetWidth");
  SkyInStartProgress = EnsureGetMember<float>("SkyInStartProgress");
  SkyOutStartProgress = EnsureGetMember<float>("SkyOutStartProgress");
  SkyMoveDurationIn = EnsureGetMember<float>("SkyMoveDurationIn");
  SkyMoveDurationOut = EnsureGetMember<float>("SkyMoveDurationOut");
  EntriesMoveDurationIn = EnsureGetMember<float>("EntriesMoveDurationIn");
  EntriesMoveDurationOut = EnsureGetMember<float>("EntriesMoveDurationOut");
  HighlightDurationIn = EnsureGetMember<float>("HighlightDurationIn");
  HighlightDurationOut = EnsureGetMember<float>("HighlightDurationOut");

  ButtonBackgroundSprite = EnsureGetMember<Sprite>("ButtonBackgroundSprite");
  ButtonPromptsSprite = EnsureGetMember<Sprite>("ButtonPromptsSprite");
  ButtonBackgroundStartX = EnsureGetMember<float>("ButtonBackgroundStartX");
  ButtonBackgroundX = EnsureGetMember<float>("ButtonBackgroundX");
  ButtonBackgroundY = EnsureGetMember<float>("ButtonBackgroundY");
  ButtonBackgroundTargetWidth =
      EnsureGetMember<float>("ButtonBackgroundTargetWidth");
  ButtonBackgroundSprStartX =
      EnsureGetMember<float>("ButtonBackgroundSprStartX");

  SkyMoveAnimation.DurationIn = SkyMoveDurationIn;
  SkyMoveAnimation.DurationOut = SkyMoveDurationOut;

  EntriesMoveAnimation.DurationIn = EntriesMoveDurationIn;
  EntriesMoveAnimation.DurationOut = EntriesMoveDurationOut;

  HighlightAnimation.DurationIn = HighlightDurationIn;
  HighlightAnimation.DurationOut = HighlightDurationOut;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::SystemMenuPtr = new UI::RNE::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto