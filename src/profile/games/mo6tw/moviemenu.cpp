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
