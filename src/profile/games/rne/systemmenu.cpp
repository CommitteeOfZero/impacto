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

Sprite SkyBackgroundSprite;
Sprite SkyArrowSprite;
Sprite SkyTextSprite;
Sprite ButtonBackgroundSprite;
Sprite ButtonPromptsSprite;
float ButtonBackgroundStartX;
float ButtonBackgroundX;
float ButtonBackgroundY;
float ButtonBackgroundTargetWidth;
float ButtonBackgroundSprStartX;
float SkyBackgroundBeginX;
float SkyBackgroundY;
float SkyTextBeginX;
float SkyTextY;
float MenuEntriesXSkew;
float MenuEntriesTargetWidth;
float SkyInStartProgress;
float SkyOutStartProgress;
float SkyMoveDurationIn;
float SkyMoveDurationOut;
float EntriesMoveDurationIn;
float EntriesMoveDurationOut;
float HighlightDurationIn;
float HighlightDurationOut;
Animation* BackgroundAnimation = 0;
Animation SkyMoveAnimation;
Animation EntriesMoveAnimation;
Animation HighlightAnimation;

void Configure() {
  EnsurePushMemberOfType("Background", LUA_TTABLE);
  BackgroundAnimation = RNE::ParseTileBackground();
  RNE::ParseTileBackground();
  Pop();

  SkyBackgroundSprite = EnsureGetMemberSprite("SkyBackgroundSprite");
  SkyArrowSprite = EnsureGetMemberSprite("SkyArrowSprite");
  SkyTextSprite = EnsureGetMemberSprite("SkyTextSprite");
  SkyBackgroundBeginX = EnsureGetMemberFloat("SkyBackgroundBeginX");
  SkyBackgroundY = EnsureGetMemberFloat("SkyBackgroundY");
  SkyTextBeginX = EnsureGetMemberFloat("SkyTextBeginX");
  SkyTextY = EnsureGetMemberFloat("SkyTextY");
  MenuEntriesXSkew = EnsureGetMemberFloat("MenuEntriesXSkew");
  MenuEntriesTargetWidth = EnsureGetMemberFloat("MenuEntriesTargetWidth");
  SkyInStartProgress = EnsureGetMemberFloat("SkyInStartProgress");
  SkyOutStartProgress = EnsureGetMemberFloat("SkyOutStartProgress");
  SkyMoveDurationIn = EnsureGetMemberFloat("SkyMoveDurationIn");
  SkyMoveDurationOut = EnsureGetMemberFloat("SkyMoveDurationOut");
  EntriesMoveDurationIn = EnsureGetMemberFloat("EntriesMoveDurationIn");
  EntriesMoveDurationOut = EnsureGetMemberFloat("EntriesMoveDurationOut");
  HighlightDurationIn = EnsureGetMemberFloat("HighlightDurationIn");
  HighlightDurationOut = EnsureGetMemberFloat("HighlightDurationOut");

  ButtonBackgroundSprite = EnsureGetMemberSprite("ButtonBackgroundSprite");
  ButtonPromptsSprite = EnsureGetMemberSprite("ButtonPromptsSprite");
  ButtonBackgroundStartX = EnsureGetMemberFloat("ButtonBackgroundStartX");
  ButtonBackgroundX = EnsureGetMemberFloat("ButtonBackgroundX");
  ButtonBackgroundY = EnsureGetMemberFloat("ButtonBackgroundY");
  ButtonBackgroundTargetWidth =
      EnsureGetMemberFloat("ButtonBackgroundTargetWidth");
  ButtonBackgroundSprStartX = EnsureGetMemberFloat("ButtonBackgroundSprStartX");

  SkyMoveAnimation.DurationIn = SkyMoveDurationIn;
  SkyMoveAnimation.DurationOut = SkyMoveDurationOut;

  EntriesMoveAnimation.DurationIn = EntriesMoveDurationIn;
  EntriesMoveAnimation.DurationOut = EntriesMoveDurationOut;

  HighlightAnimation.DurationIn = HighlightDurationIn;
  HighlightAnimation.DurationOut = HighlightDurationOut;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SystemMenuPtr = new UI::RNE::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto