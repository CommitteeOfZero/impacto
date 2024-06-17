#pragma once

#include "../sprites.h"
#include "../../hud/datedisplay.h"

namespace Impacto {
namespace Profile {
namespace DateDisplay {

int constexpr NumSpriteCount = 10;
int constexpr WeekSpriteCount = 7;

inline Sprite MonthNumSprites[NumSpriteCount];
inline Sprite DayNumSprites[NumSpriteCount];
inline Sprite YearNumSprites[NumSpriteCount];
inline Sprite WeekSprites[WeekSpriteCount];

inline Sprite MDSeparatorSprite;
inline Sprite DYSeparatorSprite;
inline Sprite OpenBracketSprite;
inline Sprite CloseBracketSprite;
inline Sprite BackgroundSprite;

inline glm::vec2 BackgroundStartPos;
inline glm::vec2 BackgroundEndPos;
inline float DateStartX;
inline float YearWeekY;
inline float MonthDayY;
inline float Spacing;
inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace DateDisplay
}  // namespace Profile
}  // namespace Impacto