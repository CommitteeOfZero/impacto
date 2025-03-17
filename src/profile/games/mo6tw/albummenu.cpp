#include "albummenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/albummenu.h"
#include "../../../text.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace AlbumMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  GetMemberArray<Sprite>(CharacterButtonSprites, CharacterButtonCount,
                         "CharacterButtons");
  GetMemberArray<Sprite>(HighlightedCharacterButtonSprites,
                         CharacterButtonCount, "HighlightedCharacterButtons");
  InitialButtonPosition = EnsureGetMember<glm::vec2>("InitialButtonPosition");
  ButtonOddX = EnsureGetMember<float>("ButtonOddX");
  ButtonEvenX = EnsureGetMember<float>("ButtonEvenX");
  ButtonMargin = EnsureGetMember<glm::vec2>("ButtonMargin");
  HighlightAnimationDuration =
      EnsureGetMember<float>("HighlightAnimationDuration");
  YunoButtonIdx = EnsureGetMember<int>("YunoButtonIdx");
  SuzuButtonIdx = EnsureGetMember<int>("SuzuButtonIdx");
  GetMemberArray<Sprite>(CharacterPortraits, CharacterPortraitCount,
                         "CharacterPortraits");
  OthersPortraitTopPart = EnsureGetMember<Sprite>("OthersPortraitTopPart");
  OthersPortraitBottomPart =
      EnsureGetMember<Sprite>("OthersPortraitBottomPart");
  PortraitPosition = EnsureGetMember<glm::vec2>("PortraitPosition");
  OthersPortraitPosition = EnsureGetMember<glm::vec2>("OthersPortraitPosition");
  ThumbnailsPerRow = EnsureGetMember<int>("ThumbnailsPerRow");
  ThumbnailsPerColumn = EnsureGetMember<int>("ThumbnailsPerColumn");
  GetMemberArray<Sprite>(Thumbnails, EventCgCount, "Thumbnails");
  GetMemberArray<int>(ThumbnailOffsets, CharacterCount, "ThumbnailOffsets");
  LockedThumbnail = EnsureGetMember<Sprite>("LockedThumbnail");
  ThumbnailBorder = EnsureGetMember<Sprite>("ThumbnailBorder");
  ThumbnailHighlightTopLeft =
      EnsureGetMember<Sprite>("ThumbnailHighlightTopLeft");
  ThumbnailHighlightTopRight =
      EnsureGetMember<Sprite>("ThumbnailHighlightTopRight");
  ThumbnailHighlightBottomLeft =
      EnsureGetMember<Sprite>("ThumbnailHighlightBottomLeft");
  ThumbnailHighlightBottomRight =
      EnsureGetMember<Sprite>("ThumbnailHighlightBottomRight");
  ThumbnailGridFirstPosition =
      EnsureGetMember<glm::vec2>("ThumbnailGridFirstPosition");
  ThumbnailGridMargin = EnsureGetMember<glm::vec2>("ThumbnailGridMargin");
  ThumbnailGridBounds = EnsureGetMember<RectF>("ThumbnailGridBounds");
  ArrowsAnimationDuration = EnsureGetMember<float>("ArrowsAnimationDuration");
  ArrowUp = EnsureGetMember<Sprite>("ArrowUp");
  ArrowUpPosition = EnsureGetMember<glm::vec2>("ArrowUpPosition");
  ArrowDown = EnsureGetMember<Sprite>("ArrowDown");
  ArrowDownPosition = EnsureGetMember<glm::vec2>("ArrowDownPosition");
  ThumbnailButtonBorderOffset =
      EnsureGetMember<glm::vec2>("ThumbnailButtonBorderOffset");
  ThumbnailButtonTextFontSize =
      EnsureGetMember<float>("ThumbnailButtonTextFontSize");
  ThumbnailButtonTextColorIndex =
      EnsureGetMember<int>("ThumbnailButtonTextColorIndex");
  ThumbnailButtonTextOffset =
      EnsureGetMember<glm::vec2>("ThumbnailButtonTextOffset");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::AlbumMenu());
}

}  // namespace AlbumMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
