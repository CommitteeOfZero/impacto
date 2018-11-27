#pragma once

#include "../sprites.h"

namespace Impacto {
namespace Profile {
namespace DateDisplay {

int const NumSpriteCount = 10;
int const WeekSpriteCount = 7;

extern Sprite MonthNumSprites[NumSpriteCount];
extern Sprite DayNumSprites[NumSpriteCount];
extern Sprite YearNumSprites[NumSpriteCount];
extern Sprite WeekSprites[WeekSpriteCount];

extern Sprite MDSeparatorSprite;
extern Sprite DYSeparatorSprite;
extern Sprite OpenBracketSprite;
extern Sprite CloseBracketSprite;
extern Sprite BackgroundSprite;

extern glm::vec2 BackgroundStartPos;
extern glm::vec2 BackgroundEndPos;
extern float DateStartX;
extern float YearWeekY;
extern float MonthDayY;
extern float Spacing;
extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace DateDisplay
}  // namespace Profile
}  // namespace Impacto