#pragma once

#include "../../../spritesheet.h"
#include "../../../games/rne/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace SystemMenu {

inline Sprite SkyBackgroundSprite;
inline Sprite SkyArrowSprite;
inline Sprite SkyTextSprite;
inline Sprite ButtonBackgroundSprite;
inline Sprite ButtonPromptsSprite;
inline float ButtonBackgroundStartX;
inline float ButtonBackgroundX;
inline float ButtonBackgroundY;
inline float ButtonBackgroundTargetWidth;
inline float ButtonBackgroundSprStartX;
inline float SkyBackgroundBeginX;
inline float SkyBackgroundY;
inline float SkyTextBeginX;
inline float SkyTextY;
inline float MenuEntriesXSkew;
inline float MenuEntriesTargetWidth;
inline float SkyInStartProgress;
inline float SkyOutStartProgress;
inline float SkyMoveDurationIn;
inline float SkyMoveDurationOut;
inline float EntriesMoveDurationIn;
inline float EntriesMoveDurationOut;
inline float HighlightDurationIn;
inline float HighlightDurationOut;
inline Animation* BackgroundAnimation = nullptr;
inline Animation SkyMoveAnimation;
inline Animation EntriesMoveAnimation;
inline Animation HighlightAnimation;

void Configure();

}  // namespace SystemMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto