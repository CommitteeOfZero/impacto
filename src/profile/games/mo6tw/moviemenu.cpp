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
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  FirstOPTopPartSprite = EnsureGetMember<Sprite>("FirstOPTopPartSprite");
  FirstOPBottomPartSprite = EnsureGetMember<Sprite>("FirstOPBottomPartSprite");
  SecondOPTopPartSprite = EnsureGetMember<Sprite>("SecondOPTopPartSprite");
  SecondOPBottomPartSprite =
      EnsureGetMember<Sprite>("SecondOPBottomPartSprite");
  GetMemberArray<Sprite>(UnlockedMovieThumbnailSprites, MaxMovieThumbnails,
                         "UnlockedMovieThumbnailSprites");
  GetMemberArray<Sprite>(LockedMovieThumbnailSprites, MaxMovieThumbnails,
                         "LockedMovieThumbnailSprites");
  SelectionHighlightTopLeft =
      EnsureGetMember<Sprite>("SelectionHighlightTopLeft");
  SelectionHighlightTopRight =
      EnsureGetMember<Sprite>("SelectionHighlightTopRight");
  SelectionHighlightBottomLeft =
      EnsureGetMember<Sprite>("SelectionHighlightBottomLeft");
  SelectionHighlightBottomRight =
      EnsureGetMember<Sprite>("SelectionHighlightBottomRight");
  ItemCount = EnsureGetMember<int>("ItemCount");
  ItemsPerRow = EnsureGetMember<int>("ItemsPerRow");
  InitialItemPosition = EnsureGetMember<glm::vec2>("InitialItemPosition");
  ItemOffset = EnsureGetMember<glm::vec2>("ItemOffset");

  HighlightAnimationDuration =
      EnsureGetMember<float>("HighlightAnimationDuration");
  HighlightTopLeftOffset = EnsureGetMember<glm::vec2>("HighlightTopLeftOffset");
  HighlightTopRightOffset =
      EnsureGetMember<glm::vec2>("HighlightTopRightOffset");
  HighlightBottomLeftOffset =
      EnsureGetMember<glm::vec2>("HighlightBottomLeftOffset");
  HighlightBottomRightOffset =
      EnsureGetMember<glm::vec2>("HighlightBottomRightOffset");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::MovieMenu());
}

}  // namespace MovieMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
