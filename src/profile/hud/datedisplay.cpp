#include "datedisplay.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace DateDisplay {

Sprite MonthNumSprites[NumSpriteCount];
Sprite DayNumSprites[NumSpriteCount];
Sprite YearNumSprites[NumSpriteCount];
Sprite WeekSprites[WeekSpriteCount];

Sprite MDSeparatorSprite;
Sprite DYSeparatorSprite;
Sprite OpenBracketSprite;
Sprite CloseBracketSprite;
Sprite BackgroundSprite;

float BackgroundStartX;
float BackgroundY;
float DateStartX;
float YearWeekY;
float MonthDayY;
float Spacing;
float FadeInDuration;
float FadeOutDuration;

void Configure() {
  auto const& _dateDisplay =
      EnsureGetMemberOfType(Json, "/", "DateDisplay", kObjectType);
  auto const& monthSprites = EnsureGetMemberOfType(
      _dateDisplay, "/DateDisplay/x", "MonthNumSprites", kArrayType);
  auto const& daySprites = EnsureGetMemberOfType(_dateDisplay, "/DateDisplay/x",
                                                 "DayNumSprites", kArrayType);
  auto const& yearSprites = EnsureGetMemberOfType(
      _dateDisplay, "/DateDisplay/x", "YearNumSprites", kArrayType);

  assert(monthSprites.Size() == NumSpriteCount);
  assert(daySprites.Size() == NumSpriteCount);
  assert(yearSprites.Size() == NumSpriteCount);

  for (int i = 0; i < NumSpriteCount; i++) {
    MonthNumSprites[i] =
        EnsureGetSprite(monthSprites[i], "/DateDisplay/x/MonthNumSprites/y");
    DayNumSprites[i] =
        EnsureGetSprite(daySprites[i], "/DateDisplay/x/DayNumSprites/y");
    YearNumSprites[i] =
        EnsureGetSprite(yearSprites[i], "/DateDisplay/x/YearNumSprites/y");
  }

  auto const& weekSprites = EnsureGetMemberOfType(
      _dateDisplay, "/DateDisplay/x", "WeekSprites", kArrayType);

  assert(weekSprites.Size() == WeekSpriteCount);

  for (int i = 0; i < WeekSpriteCount; i++) {
    WeekSprites[i] =
        EnsureGetSprite(weekSprites[i], "/DateDisplay/x/WeekSprites/y");
  }

  MDSeparatorSprite =
      EnsureGetMemberSprite(_dateDisplay, "/DateDisplay", "MDSeparatorSprite");
  DYSeparatorSprite =
      EnsureGetMemberSprite(_dateDisplay, "/DateDisplay", "DYSeparatorSprite");
  OpenBracketSprite =
      EnsureGetMemberSprite(_dateDisplay, "/DateDisplay", "OpenBracketSprite");
  CloseBracketSprite =
      EnsureGetMemberSprite(_dateDisplay, "/DateDisplay", "CloseBracketSprite");
  BackgroundSprite =
      EnsureGetMemberSprite(_dateDisplay, "/DateDisplay", "BackgroundSprite");

  BackgroundStartX =
      EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "BackgroundStartX");
  BackgroundY =
      EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "BackgroundY");
  DateStartX = EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "DateStartX");
  YearWeekY = EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "YearWeekY");
  MonthDayY = EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "MonthDayY");
  Spacing = EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "Spacing");
  FadeInDuration =
      EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "FadeInDuration");
  FadeOutDuration =
      EnsureGetMemberFloat(_dateDisplay, "/DateDisplay", "FadeOutDuration");
}

}  // namespace DateDisplay
}  // namespace Profile
}  // namespace Impacto