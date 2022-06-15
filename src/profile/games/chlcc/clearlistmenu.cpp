#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/clearlistmenu.h"
#include "../../../window.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace ClearListMenu {

uint32_t BackgroundColor;
Sprite BackgroundFilter;
glm::vec2 ErinPosition;
Sprite ErinSprite;

glm::vec2 LabelPosition;
Sprite ClearListLabel;
glm::vec2 TimePositions[6];
glm::vec2 EndingCountPosition;
glm::vec2 TIPSCountPositions[2];
glm::vec2 AlbumPositions[3];
Sprite Digits[10];
glm::vec2 ListPosition;
Sprite EndingList;
glm::vec2 BoxPositions[Endings];
Sprite EndingBox;
glm::vec2 ThumbnailPositions[Endings];
Sprite EndingThumbnails[Endings];
Sprite LockedThumbnail;

static void GetVec2SpriteArray(glm::vec2* arr, uint32_t count,
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
  BackgroundColor = EnsureGetMemberUint("BackgroundColor");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");

  LabelPosition = EnsureGetMemberVec2("LabelPosition");
  ClearListLabel = EnsureGetMemberSprite("ClearListLabel");
  GetVec2SpriteArray(TimePositions, 6, "TimePositions");
  EndingCountPosition = EnsureGetMemberVec2("EndingCountPosition");
  GetVec2SpriteArray(TIPSCountPositions, 2, "TIPSCountPositions");
  GetMemberSpriteArray(Digits, 10, "Digits");
  GetVec2SpriteArray(AlbumPositions, 3, "AlbumPositions");
  ListPosition = EnsureGetMemberVec2("ListPosition");
  EndingList = EnsureGetMemberSprite("EndingList");
  GetVec2SpriteArray(BoxPositions, Endings, "BoxPositions");
  EndingBox = EnsureGetMemberSprite("EndingBox");
  GetVec2SpriteArray(ThumbnailPositions, Endings, "ThumbnailPositions");
  GetMemberSpriteArray(EndingThumbnails, Endings, "EndingThumbnails");
  LockedThumbnail = EnsureGetMemberSprite("LockedThumbnail");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  auto clearList = new UI::CHLCC::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto