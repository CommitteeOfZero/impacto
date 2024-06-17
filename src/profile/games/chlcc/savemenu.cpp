#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/savemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SaveMenu {

void Configure() {
  EntryStartX = EnsureGetMemberFloat("EntryStartX");
  EntryXPadding = EnsureGetMemberFloat("EntryXPadding");
  EntryStartY = EnsureGetMemberFloat("EntryStartY");
  EntryYPadding = EnsureGetMemberFloat("EntryYPadding");
  SaveBackgroundColor = EnsureGetMemberUint("SaveBackgroundColor");
  LoadBackgroundColor = EnsureGetMemberUint("LoadBackgroundColor");
  QuickLoadBackgroundColor = EnsureGetMemberUint("QuickLoadBackgroundColor");
  SaveCircle = EnsureGetMemberSprite("SaveCircle");
  LoadCircle = EnsureGetMemberSprite("LoadCircle");
  QuickLoadCircle = EnsureGetMemberSprite("QuickLoadCircle");
  CircleStartPosition = EnsureGetMemberVec2("CircleStartPosition");
  CircleOffset = EnsureGetMemberFloat("CircleOffset");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMemberVec2("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMemberVec2("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMemberSprite("RedBarSprite");
  RedBarDivision = EnsureGetMemberFloat("RedBarDivision");
  RedBarBaseX = EnsureGetMemberFloat("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMemberVec2("RedBarLabelPosition");
  RedBarLabel = EnsureGetMemberSprite("RedBarLabel");
  QuickLoadTextSprite = EnsureGetMemberSprite("QuickLoadTextSprite");
  LoadTextSprite = EnsureGetMemberSprite("LoadTextSprite");
  SaveTextSprite = EnsureGetMemberSprite("SaveTextSprite");
  MenuTitleTextRightPos = EnsureGetMemberVec2("MenuTitleTextRightPos");
  MenuTitleTextLeftPos = EnsureGetMemberVec2("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMemberFloat("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");

  SaveListPosition = EnsureGetMemberVec2("SaveListPosition");
  SaveListSprite = EnsureGetMemberSprite("SaveListSprite");
  GetMemberVec2Array(EntryPositions, EntriesPerPage, "EntryPositions");
  QuickLoadEntrySprite = EnsureGetMemberSprite("QuickLoadEntrySprite");
  SaveEntrySprite = EnsureGetMemberSprite("SaveEntrySprite");
  LoadEntrySprite = EnsureGetMemberSprite("LoadEntrySprite");
  EntryHighlightedSprite = EnsureGetMemberSprite("EntryHighlightedSprite");
  LockedSymbolSprite = EnsureGetMemberSprite("LockedSymbolSprite");
  ThumbnailRelativePos = EnsureGetMemberVec2("ThumbnailRelativePos");
  PageNumBackgroundPos = EnsureGetMemberVec2("PageNumBackgroundPos");
  PageNumBackgroundSprite = EnsureGetMemberSprite("PageNumBackground");
  CurrentPageNumPos = EnsureGetMemberVec2("CurrentPageNumPos");
  GetMemberSpriteArray(BigDigits, 10, "BigDigits");
  PageNumSeparatorSlashPos = EnsureGetMemberVec2("PageNumSeparatorSlashPos");
  PageNumSeparatorSlashSprite = EnsureGetMemberSprite("PageNumSeparatorSlash");
  MaxPageNumPos = EnsureGetMemberVec2("MaxPageNumPos");
  MaxPageNumSprite = EnsureGetMemberSprite("MaxPageNum");
  ButtonPromptPosition = EnsureGetMemberVec2("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMemberSprite("ButtonPrompt");
  SelectDataFadeDuration = EnsureGetMemberFloat("SelectDataFadeDuration");
  // 10 letters in "SELECT DATA"
  GetMemberVec2Array(SelectDataTextPositions, 10, "SelectDataTextPositions");
  GetMemberSpriteArray(SelectDataTextSprites, 10, "SelectDataText");

  EntryNumberHintTextRelativePos =
      EnsureGetMemberVec2("EntryNumberHintTextRelativePos");
  EntryNumberTextRelativePos =
      EnsureGetMemberVec2("EntryNumberTextRelativePos");
  SceneTitleTextRelativePos = EnsureGetMemberVec2("SceneTitleTextRelativePos");
  NoDataTextRelativePos = EnsureGetMemberVec2("NoDataTextRelativePos");
  PlayTimeHintTextRelativePos =
      EnsureGetMemberVec2("PlayTimeHintTextRelativePos");
  PlayTimeTextRelativePos = EnsureGetMemberVec2("PlayTimeTextRelativePos");
  SaveDateHintTextRelativePos =
      EnsureGetMemberVec2("SaveDateHintTextRelativePos");
  SaveDateTextRelativePos = EnsureGetMemberVec2("SaveDateTextRelativePos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SaveMenuPtr = new UI::CHLCC::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
