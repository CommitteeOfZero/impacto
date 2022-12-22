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

uint32_t BackgroundColor;
uint32_t SaveBackgroundColor;
uint32_t LoadBackgroundColor;
uint32_t QuickLoadBackgroundColor;
Sprite CircleSprite;
Sprite SaveCircle;
Sprite LoadCircle;
Sprite QuickLoadCircle;
glm::vec2 CircleStartPosition;
float CircleOffset;
glm::vec2 ErinPosition;
Sprite ErinSprite;
Sprite BackgroundFilter;
glm::vec2 InitialRedBarPosition;
glm::vec2 RightRedBarPosition;
Sprite InitialRedBarSprite;
glm::vec2 RedBarPosition;
float RedBarDivision;
float RedBarBaseX;
Sprite RedBarSprite;
glm::vec2 RedBarLabelPosition;
Sprite RedBarLabel;
Sprite MenuTitleTextSprite;
Sprite QuickLoadTextSprite;
Sprite LoadTextSprite;
Sprite SaveTextSprite;
glm::vec2 MenuTitleTextLeftPos;
glm::vec2 MenuTitleTextRightPos;
float MenuTitleTextAngle;
float TitleFadeInDuration;
float TitleFadeOutDuration;

glm::vec2 SaveListPosition;
Sprite SaveListSprite;
glm::vec2 EntryPositions[EntriesPerPage];
Sprite QuickLoadEntrySprite;
Sprite SaveEntrySprite;
Sprite LoadEntrySprite;
Sprite EntryHighlightedSprite;
Sprite LockedSymbolSprite;
glm::vec2 ThumbnailRelativePos;
glm::vec2 PageNumBackgroundPos;
Sprite PageNumBackgroundSprite;
glm::vec2 CurrentPageNumPos;
Sprite BigDigits[10];
glm::vec2 PageNumSeparatorSlashPos;
Sprite PageNumSeparatorSlashSprite;
glm::vec2 MaxPageNumPos;
Sprite MaxPageNumSprite;
glm::vec2 ButtonPromptPosition;
Sprite ButtonPromptSprite;
float SelectDataFadeDuration;
// 10 letters in "SELECT DATA"
glm::vec2 SelectDataTextPositions[10];
Sprite SelectDataTextSprites[10];

glm::vec2 EntryNumberHintTextRelativePos;
glm::vec2 EntryNumberTextRelativePos;
glm::vec2 SceneTitleTextRelativePos;
glm::vec2 NoDataTextRelativePos;
glm::vec2 PlayTimeHintTextRelativePos;
glm::vec2 PlayTimeTextRelativePos;
glm::vec2 SaveDateHintTextRelativePos;
glm::vec2 SaveDateTextRelativePos;

static void GetMemberVec2Array(glm::vec2* arr, uint32_t count,
                               char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d vec2 for %s\n", count,
           name);
    Renderer->Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementVec2(i);
  }

  Pop();
}

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Renderer->Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
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
