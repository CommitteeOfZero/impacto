#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/savemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SaveMenu {

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
glm::vec2 ThumbnailRelativePos;

static void GetMemberVec2Array(glm::vec2* arr, uint32_t count,
                               char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d vec2 for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementVec2(i);
  }

  Pop();
}

void Configure() {
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
  ThumbnailRelativePos = EnsureGetMemberVec2("ThumbnailRelativePos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SaveMenuPtr = new UI::CHLCC::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
