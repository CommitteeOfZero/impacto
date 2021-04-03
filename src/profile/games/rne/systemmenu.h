#pragma once

#include "../../../spritesheet.h"
#include "../../../games/rne/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace SystemMenu {

extern Sprite SkyBackgroundSprite;
extern Sprite SkyArrowSprite;
extern Sprite SkyTextSprite;
extern Sprite ButtonBackgroundSprite;
extern Sprite ButtonPromptsSprite;
extern float ButtonBackgroundStartX;
extern float ButtonBackgroundX;
extern float ButtonBackgroundY;
extern float ButtonBackgroundTargetWidth;
extern float ButtonBackgroundSprStartX;
extern float SkyBackgroundBeginX;
extern float SkyBackgroundY;
extern float SkyTextBeginX;
extern float SkyTextY;
extern float MenuEntriesXSkew;
extern float MenuEntriesTargetWidth;
extern float SkyInStartProgress;
extern float SkyOutStartProgress;
extern float SkyMoveDurationIn;
extern float SkyMoveDurationOut;
extern float EntriesMoveDurationIn;
extern float EntriesMoveDurationOut;
extern float HighlightDurationIn;
extern float HighlightDurationOut;
extern Animation* BackgroundAnimation;
extern Animation SkyMoveAnimation;
extern Animation EntriesMoveAnimation;
extern Animation HighlightAnimation;

void Configure();

}  // namespace SystemMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto