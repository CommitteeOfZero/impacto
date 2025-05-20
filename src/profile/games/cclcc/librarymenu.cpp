#include "librarymenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/librarymenu.h"
#include "../../../text.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace LibraryMenu {

void Configure() {
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  LibraryBackgroundSprite = EnsureGetMember<Sprite>("LibraryBackgroundSprite");
  LibraryBackgroundPosition =
      EnsureGetMember<glm::vec2>("LibraryBackgroundPosition");
  LibraryIndexSprite = EnsureGetMember<Sprite>("LibraryIndexSprite");
  LibraryIndexPosition = EnsureGetMember<glm::vec2>("LibraryIndexPosition");
  LibraryButtonGuideSprite =
      EnsureGetMember<Sprite>("LibraryButtonGuideSprite");
  LibraryButtonGuidePosition =
      EnsureGetMember<glm::vec2>("LibraryButtonGuidePosition");
  LibraryMaskSprite = EnsureGetMember<Sprite>("LibraryMaskSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  SnapPhotoSpriteHover = EnsureGetMember<Sprite>("SnapPhotoSpriteHover");
  SnapPhotoSpriteSelect = EnsureGetMember<Sprite>("SnapPhotoSpriteSelect");
  SnapPhotoPos = EnsureGetMember<glm::vec2>("SnapPhotoPos");
  HitSongsSpriteHover = EnsureGetMember<Sprite>("HitSongsSpriteHover");
  HitSongsSpriteSelect = EnsureGetMember<Sprite>("HitSongsSpriteSelect");
  HitSongsPos = EnsureGetMember<glm::vec2>("HitSongsPos");
  LoveMovieSpriteHover = EnsureGetMember<Sprite>("LoveMovieSpriteHover");
  LoveMovieSpriteSelect = EnsureGetMember<Sprite>("LoveMovieSpriteSelect");
  LoveMoviePos = EnsureGetMember<glm::vec2>("LoveMoviePos");

  MovieDiskSprites = GetMemberVector<Sprite>("MovieDiskSprites");
  MovieDiskHighlightSprites =
      GetMemberVector<Sprite>("MovieDiskHighlightSprites");
  MovieDiskDisplayPositions =
      GetMemberVector<glm::vec2>("MovieDiskDisplayPositions");
  SubMenuFadeInDuration = EnsureGetMember<float>("SubMenuFadeInDuration");
  SubMenuFadeOutDuration = EnsureGetMember<float>("SubMenuFadeOutDuration");
  MovieDiskPlayIds = GetMemberVector<int>("MovieDiskPlayIds");

  UI::LibraryMenuPtr = new UI::CCLCC::LibraryMenu();
  UI::Menus[drawType].push_back(UI::LibraryMenuPtr);
}

}  // namespace LibraryMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto