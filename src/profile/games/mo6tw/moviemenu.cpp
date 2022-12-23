#include "moviemenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/moviemenu.h"
#include "../../../text.h"
#include "../../../game.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MovieMenu {

Sprite BackgroundSprite;

Sprite FirstOPTopPartSprite;
Sprite FirstOPBottomPartSprite;
Sprite SecondOPTopPartSprite;
Sprite SecondOPBottomPartSprite;
Sprite UnlockedMovieThumbnailSprites[MaxMovieThumbnails];
Sprite LockedMovieThumbnailSprites[MaxMovieThumbnails];
Sprite SelectionHighlightTopLeft;
Sprite SelectionHighlightTopRight;
Sprite SelectionHighlightBottomLeft;
Sprite SelectionHighlightBottomRight;
int ItemCount;
int ItemsPerRow;
glm::vec2 InitialItemPosition;
glm::vec2 ItemOffset;

float HighlightAnimationDuration;
glm::vec2 HighlightTopLeftOffset;
glm::vec2 HighlightTopRightOffset;
glm::vec2 HighlightBottomLeftOffset;
glm::vec2 HighlightBottomRightOffset;

float FadeInDuration;
float FadeOutDuration;

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  FirstOPTopPartSprite = EnsureGetMemberSprite("FirstOPTopPartSprite");
  FirstOPBottomPartSprite = EnsureGetMemberSprite("FirstOPBottomPartSprite");
  SecondOPTopPartSprite = EnsureGetMemberSprite("SecondOPTopPartSprite");
  SecondOPBottomPartSprite = EnsureGetMemberSprite("SecondOPBottomPartSprite");
  GetMemberSpriteArray(UnlockedMovieThumbnailSprites, MaxMovieThumbnails,
                       "UnlockedMovieThumbnailSprites");
  GetMemberSpriteArray(LockedMovieThumbnailSprites, MaxMovieThumbnails,
                       "LockedMovieThumbnailSprites");
  SelectionHighlightTopLeft =
      EnsureGetMemberSprite("SelectionHighlightTopLeft");
  SelectionHighlightTopRight =
      EnsureGetMemberSprite("SelectionHighlightTopRight");
  SelectionHighlightBottomLeft =
      EnsureGetMemberSprite("SelectionHighlightBottomLeft");
  SelectionHighlightBottomRight =
      EnsureGetMemberSprite("SelectionHighlightBottomRight");
  ItemCount = EnsureGetMemberInt("ItemCount");
  ItemsPerRow = EnsureGetMemberInt("ItemsPerRow");
  InitialItemPosition = EnsureGetMemberVec2("InitialItemPosition");
  ItemOffset = EnsureGetMemberVec2("ItemOffset");

  HighlightAnimationDuration =
      EnsureGetMemberFloat("HighlightAnimationDuration");
  HighlightTopLeftOffset = EnsureGetMemberVec2("HighlightTopLeftOffset");
  HighlightTopRightOffset = EnsureGetMemberVec2("HighlightTopRightOffset");
  HighlightBottomLeftOffset = EnsureGetMemberVec2("HighlightBottomLeftOffset");
  HighlightBottomRightOffset =
      EnsureGetMemberVec2("HighlightBottomRightOffset");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::MovieMenu());
}

}  // namespace MovieMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
