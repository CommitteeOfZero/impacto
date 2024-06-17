#include "datedisplay.h"
#include "../profile_internal.h"
#include "../../log.h"
#include "../../renderer/renderer.h"
#include "../../games/rne/datedisplay.h"

namespace Impacto {
namespace Profile {
namespace DateDisplay {

using namespace Impacto::DateDisplay;

DateDisplayType Type = DateDisplayType::None;

void Configure() {
  EnsurePushMemberOfType("DateDisplay", LUA_TTABLE);

  Type = DateDisplayType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  if (Type == +DateDisplayType::RNE) {
    Impacto::DateDisplay::Implementation = new Impacto::RNE::DateDisplay;
  }

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