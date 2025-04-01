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

  Type =
      DateDisplayType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

  if (Type == +DateDisplayType::RNE) {
    Impacto::DateDisplay::Implementation = new Impacto::RNE::DateDisplay;
  }

  GetMemberArray<Sprite>(MonthNumSprites, NumSpriteCount, "MonthNumSprites");
  GetMemberArray<Sprite>(DayNumSprites, NumSpriteCount, "DayNumSprites");
  GetMemberArray<Sprite>(YearNumSprites, NumSpriteCount, "YearNumSprites");
  GetMemberArray<Sprite>(WeekSprites, WeekSpriteCount, "WeekSprites");

  MDSeparatorSprite = EnsureGetMember<Sprite>("MDSeparatorSprite");
  DYSeparatorSprite = EnsureGetMember<Sprite>("DYSeparatorSprite");
  OpenBracketSprite = EnsureGetMember<Sprite>("OpenBracketSprite");
  CloseBracketSprite = EnsureGetMember<Sprite>("CloseBracketSprite");
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  BackgroundStartPos = EnsureGetMember<glm::vec2>("BackgroundStartPos");
  BackgroundEndPos = EnsureGetMember<glm::vec2>("BackgroundEndPos");
  DateStartX = EnsureGetMember<float>("DateStartX");
  YearWeekY = EnsureGetMember<float>("YearWeekY");
  MonthDayY = EnsureGetMember<float>("MonthDayY");
  Spacing = EnsureGetMember<float>("Spacing");
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  Pop();
}

}  // namespace DateDisplay
}  // namespace Profile
}  // namespace Impacto