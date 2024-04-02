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

Sprite BackgroundSprite;

Sprite CharacterButtonSprites[CharacterButtonCount];
Sprite HighlightedCharacterButtonSprites[CharacterButtonCount];
glm::vec2 InitialButtonPosition;
float ButtonOddX;
float ButtonEvenX;
glm::vec2 ButtonMargin;
float HighlightAnimationDuration;
int YunoButtonIdx;
int SuzuButtonIdx;
Sprite CharacterPortraits[CharacterPortraitCount];
Sprite OthersPortraitTopPart;
Sprite OthersPortraitBottomPart;
glm::vec2 PortraitPosition;
glm::vec2 OthersPortraitPosition;
int ThumbnailsPerRow;
int ThumbnailsPerColumn;
Sprite Thumbnails[EventCgCount];
int ThumbnailOffsets[CharacterCount];
Sprite LockedThumbnail;
Sprite ThumbnailBorder;
Sprite ThumbnailHighlightTopLeft;
Sprite ThumbnailHighlightTopRight;
Sprite ThumbnailHighlightBottomLeft;
Sprite ThumbnailHighlightBottomRight;
glm::vec2 ThumbnailGridFirstPosition;
glm::vec2 ThumbnailGridMargin;
RectF ThumbnailGridBounds;
float ArrowsAnimationDuration;
Sprite ArrowUp;
glm::vec2 ArrowUpPosition;
Sprite ArrowDown;
glm::vec2 ArrowDownPosition;
glm::vec2 ThumbnailButtonBorderOffset;
float ThumbnailButtonTextFontSize;
int ThumbnailButtonTextColorIndex;
glm::vec2 ThumbnailButtonTextOffset;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  GetMemberSpriteArray(CharacterButtonSprites, CharacterButtonCount,
                       "CharacterButtons");
  GetMemberSpriteArray(HighlightedCharacterButtonSprites, CharacterButtonCount,
                       "HighlightedCharacterButtons");
  InitialButtonPosition = EnsureGetMemberVec2("InitialButtonPosition");
  ButtonOddX = EnsureGetMemberFloat("ButtonOddX");
  ButtonEvenX = EnsureGetMemberFloat("ButtonEvenX");
  ButtonMargin = EnsureGetMemberVec2("ButtonMargin");
  HighlightAnimationDuration =
      EnsureGetMemberFloat("HighlightAnimationDuration");
  YunoButtonIdx = EnsureGetMemberInt("YunoButtonIdx");
  SuzuButtonIdx = EnsureGetMemberInt("SuzuButtonIdx");
  GetMemberSpriteArray(CharacterPortraits, CharacterPortraitCount,
                       "CharacterPortraits");
  OthersPortraitTopPart = EnsureGetMemberSprite("OthersPortraitTopPart");
  OthersPortraitBottomPart = EnsureGetMemberSprite("OthersPortraitBottomPart");
  PortraitPosition = EnsureGetMemberVec2("PortraitPosition");
  OthersPortraitPosition = EnsureGetMemberVec2("OthersPortraitPosition");
  ThumbnailsPerRow = EnsureGetMemberInt("ThumbnailsPerRow");
  ThumbnailsPerColumn = EnsureGetMemberInt("ThumbnailsPerColumn");
  GetMemberSpriteArray(Thumbnails, EventCgCount, "Thumbnails");
  GetMemberIntArray(ThumbnailOffsets, CharacterCount, "ThumbnailOffsets");
  LockedThumbnail = EnsureGetMemberSprite("LockedThumbnail");
  ThumbnailBorder = EnsureGetMemberSprite("ThumbnailBorder");
  ThumbnailHighlightTopLeft =
      EnsureGetMemberSprite("ThumbnailHighlightTopLeft");
  ThumbnailHighlightTopRight =
      EnsureGetMemberSprite("ThumbnailHighlightTopRight");
  ThumbnailHighlightBottomLeft =
      EnsureGetMemberSprite("ThumbnailHighlightBottomLeft");
  ThumbnailHighlightBottomRight =
      EnsureGetMemberSprite("ThumbnailHighlightBottomRight");
  ThumbnailGridFirstPosition =
      EnsureGetMemberVec2("ThumbnailGridFirstPosition");
  ThumbnailGridMargin = EnsureGetMemberVec2("ThumbnailGridMargin");
  ThumbnailGridBounds = EnsureGetMemberRectF("ThumbnailGridBounds");
  ArrowsAnimationDuration = EnsureGetMemberFloat("ArrowsAnimationDuration");
  ArrowUp = EnsureGetMemberSprite("ArrowUp");
  ArrowUpPosition = EnsureGetMemberVec2("ArrowUpPosition");
  ArrowDown = EnsureGetMemberSprite("ArrowDown");
  ArrowDownPosition = EnsureGetMemberVec2("ArrowDownPosition");
  ThumbnailButtonBorderOffset =
      EnsureGetMemberVec2("ThumbnailButtonBorderOffset");
  ThumbnailButtonTextFontSize =
      EnsureGetMemberFloat("ThumbnailButtonTextFontSize");
  ThumbnailButtonTextColorIndex =
      EnsureGetMemberInt("ThumbnailButtonTextColorIndex");
  ThumbnailButtonTextOffset = EnsureGetMemberVec2("ThumbnailButtonTextOffset");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::AlbumMenu());
}

}  // namespace AlbumMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
