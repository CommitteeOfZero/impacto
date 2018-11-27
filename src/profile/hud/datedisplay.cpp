#include "datedisplay.h"
#include "../profile_internal.h"
#include "../../log.h"
#include "../../window.h"

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

glm::vec2 BackgroundStartPos;
glm::vec2 BackgroundEndPos;
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
      _dateDisplay, "/DateDisplay", "MonthNumSprites", kArrayType);
  auto const& daySprites = EnsureGetMemberOfType(_dateDisplay, "/DateDisplay",
                                                 "DayNumSprites", kArrayType);
  auto const& yearSprites = EnsureGetMemberOfType(_dateDisplay, "/DateDisplay",
                                                  "YearNumSprites", kArrayType);

  if (monthSprites.Size() != NumSpriteCount ||
      daySprites.Size() != NumSpriteCount ||
      yearSprites.Size() != NumSpriteCount) {
    ImpLog(LL_Fatal, LC_Profile,
           "Expected to have %d digit sprites for years, months and days\n",
           NumSpriteCount);
    Window::Shutdown();
  }

  for (int i = 0; i < NumSpriteCount; i++) {
    MonthNumSprites[i] =
        EnsureGetSprite(monthSprites[i], "/DateDisplay/MonthNumSprites/x");
    DayNumSprites[i] =
        EnsureGetSprite(daySprites[i], "/DateDisplay/DayNumSprites/x");
    YearNumSprites[i] =
        EnsureGetSprite(yearSprites[i], "/DateDisplay/YearNumSprites/x");
  }

  auto const& weekSprites = EnsureGetMemberOfType(_dateDisplay, "/DateDisplay",
                                                  "WeekSprites", kArrayType);

  if (weekSprites.Size() != WeekSpriteCount) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d weekday sprites\n",
           WeekSpriteCount);
    Window::Shutdown();
  }

  for (int i = 0; i < WeekSpriteCount; i++) {
    WeekSprites[i] =
        EnsureGetSprite(weekSprites[i], "/DateDisplay/WeekSprites/x");
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

  BackgroundStartPos =
      EnsureGetMemberVec2(_dateDisplay, "/DateDisplay", "BackgroundStartPos");
  BackgroundEndPos =
      EnsureGetMemberVec2(_dateDisplay, "/DateDisplay", "BackgroundEndPos");
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