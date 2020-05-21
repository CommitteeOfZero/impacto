#include "mainmenu.h"
#include "../../profile_internal.h"
#include "tilebackground.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace MainMenu {

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

Impacto::MainMenu::MainMenuBase* Configure() {
  Impacto::RNE::MainMenu* result = new Impacto::RNE::MainMenu();

  EnsurePushMemberOfType("Background", kObjectType);
  result->BackgroundAnimation = RNE::ParseTileBackground();
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

  result->SkyMoveAnimation.DurationIn = SkyMoveDurationIn;
  result->SkyMoveAnimation.DurationOut = SkyMoveDurationOut;

  result->EntriesMoveAnimation.DurationIn = EntriesMoveDurationIn;
  result->EntriesMoveAnimation.DurationOut = EntriesMoveDurationOut;

  result->HighlightAnimation.DurationIn = HighlightDurationIn;
  result->HighlightAnimation.DurationOut = HighlightDurationOut;

  return result;
}

}  // namespace MainMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto