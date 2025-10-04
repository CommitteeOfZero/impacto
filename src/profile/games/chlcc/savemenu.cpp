#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/savemenu.h"

#include "albummenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SaveMenu {

void Configure() {
  EntryStartX = EnsureGetMember<float>("EntryStartX");
  EntryXPadding = EnsureGetMember<float>("EntryXPadding");
  EntryStartY = EnsureGetMember<float>("EntryStartY");
  EntryYPadding = EnsureGetMember<float>("EntryYPadding");
  SaveBackgroundColor = EnsureGetMember<uint32_t>("SaveBackgroundColor");
  LoadBackgroundColor = EnsureGetMember<uint32_t>("LoadBackgroundColor");
  QuickLoadBackgroundColor =
      EnsureGetMember<uint32_t>("QuickLoadBackgroundColor");
  SaveCircle = EnsureGetMember<Sprite>("SaveCircle");
  LoadCircle = EnsureGetMember<Sprite>("LoadCircle");
  QuickLoadCircle = EnsureGetMember<Sprite>("QuickLoadCircle");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");
  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");
  QuickLoadTextSprite = EnsureGetMember<Sprite>("QuickLoadTextSprite");
  LoadTextSprite = EnsureGetMember<Sprite>("LoadTextSprite");
  SaveTextSprite = EnsureGetMember<Sprite>("SaveTextSprite");
  MenuTitleTextRightPos = EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPos = EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");

  SaveListPosition = EnsureGetMember<glm::vec2>("SaveListPosition");
  SaveListSprite = EnsureGetMember<Sprite>("SaveListSprite");
  GetMemberArray<glm::vec2>(EntryPositions, EntriesPerPage, "EntryPositions");
  QuickLoadEntrySprite = EnsureGetMember<Sprite>("QuickLoadEntrySprite");
  SaveEntrySprite = EnsureGetMember<Sprite>("SaveEntrySprite");
  LoadEntrySprite = EnsureGetMember<Sprite>("LoadEntrySprite");
  SelectionMarkerSprite = EnsureGetMember<Sprite>("SelectionMarkerSprite");
  SelectionMarkerOffset = EnsureGetMember<glm::vec2>("SelectionMarkerOffset");
  EntryHighlightedSprite = EnsureGetMember<Sprite>("EntryHighlightedSprite");
  LockedSymbolSprite = EnsureGetMember<Sprite>("LockedSymbolSprite");
  LockedSymbolRelativePos =
      EnsureGetMember<glm::vec2>("LockedSymbolRelativePos");
  ThumbnailRelativePos = EnsureGetMember<glm::vec2>("ThumbnailRelativePos");
  PageNumBackgroundPos = EnsureGetMember<glm::vec2>("PageNumBackgroundPos");
  PageNumBackgroundSprite = EnsureGetMember<Sprite>("PageNumBackground");
  CurrentPageNumPos = EnsureGetMember<glm::vec2>("CurrentPageNumPos");
  GetMemberArray<Sprite>(BigDigits, 10, "BigDigits");
  PageNumSeparatorSlashPos =
      EnsureGetMember<glm::vec2>("PageNumSeparatorSlashPos");
  PageNumSeparatorSlashSprite =
      EnsureGetMember<Sprite>("PageNumSeparatorSlash");
  MaxPageNumPos = EnsureGetMember<glm::vec2>("MaxPageNumPos");
  MaxPageNumSprite = EnsureGetMember<Sprite>("MaxPageNum");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPrompt");
  SelectDataFadeDuration = EnsureGetMember<float>("SelectDataFadeDuration");
  // 10 letters in "SELECT DATA"
  GetMemberArray<glm::vec2>(SelectDataTextPositions, 10,
                            "SelectDataTextPositions");
  GetMemberArray<Sprite>(SelectDataTextSprites, 10, "SelectDataText");

  EntryNumberHintTextRelativePos =
      EnsureGetMember<glm::vec2>("EntryNumberHintTextRelativePos");
  EntryNumberTextRelativePos =
      EnsureGetMember<glm::vec2>("EntryNumberTextRelativePos");
  SceneTitleTextRelativePos =
      EnsureGetMember<glm::vec2>("SceneTitleTextRelativePos");
  NoDataTextRelativePos = EnsureGetMember<glm::vec2>("NoDataTextRelativePos");
  PlayTimeHintTextRelativePos =
      EnsureGetMember<glm::vec2>("PlayTimeHintTextRelativePos");
  PlayTimeTextRelativePos =
      EnsureGetMember<glm::vec2>("PlayTimeTextRelativePos");
  SaveDateHintTextRelativePos =
      EnsureGetMember<glm::vec2>("SaveDateHintTextRelativePos");
  SaveDateTextRelativePos =
      EnsureGetMember<glm::vec2>("SaveDateTextRelativePos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::SaveMenuPtr = new UI::CHLCC::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
