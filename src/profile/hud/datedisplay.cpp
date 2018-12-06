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

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  EnsurePushMemberOfType("DateDisplay", kObjectType);

  GetMemberSpriteArray(MonthNumSprites, NumSpriteCount, "MonthNumSprites");
  GetMemberSpriteArray(DayNumSprites, NumSpriteCount, "DayNumSprites");
  GetMemberSpriteArray(YearNumSprites, NumSpriteCount, "YearNumSprites");
  GetMemberSpriteArray(WeekSprites, WeekSpriteCount, "WeekSprites");

  MDSeparatorSprite = EnsureGetMemberSprite("MDSeparatorSprite");
  DYSeparatorSprite = EnsureGetMemberSprite("DYSeparatorSprite");
  OpenBracketSprite = EnsureGetMemberSprite("OpenBracketSprite");
  CloseBracketSprite = EnsureGetMemberSprite("CloseBracketSprite");
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  BackgroundStartPos = EnsureGetMemberVec2("BackgroundStartPos");
  BackgroundEndPos = EnsureGetMemberVec2("BackgroundEndPos");
  DateStartX = EnsureGetMemberFloat("DateStartX");
  YearWeekY = EnsureGetMemberFloat("YearWeekY");
  MonthDayY = EnsureGetMemberFloat("MonthDayY");
  Spacing = EnsureGetMemberFloat("Spacing");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  Pop();
}

}  // namespace DateDisplay
}  // namespace Profile
}  // namespace Impacto