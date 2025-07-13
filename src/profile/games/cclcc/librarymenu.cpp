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
  ButtonBlinkDuration = EnsureGetMember<float>("ButtonBlinkDuration");
  ButtonBlinkTintMinimum = EnsureGetMember<float>("ButtonBlinkTintMinimum");

  LibraryTransitionPositionOffset =
      EnsureGetMember<float>("LibraryTransitionPositionOffset");
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

  MusicItemsBackgroundRepeatHeight =
      EnsureGetMember<int>("MusicItemsBackgroundRepeatHeight");
  MusicItemsBackgroundSprite =
      EnsureGetMember<Sprite>("MusicItemsBackgroundSprite");
  MusicItemsOverlaySprite = EnsureGetMember<Sprite>("MusicItemsOverlaySprite");
  MusicItemsBackgroundPosition =
      EnsureGetMember<glm::vec2>("MusicItemsBackgroundPosition");
  MusicRenderingBounds = EnsureGetMember<RectF>("MusicRenderingBounds");
  MusicHoverBounds = EnsureGetMember<RectF>("MusicHoverBounds");
  MusicPlayIds = GetMemberVector<uint8_t>("MusicPlayIds");
  MusicBGMFlagIds = GetMemberVector<uint8_t>("MusicBGMFlagIds");
  MusicStringTableId = EnsureGetMember<uint8_t>("MusicStringTableId");
  MusicStringLockedIndex = EnsureGetMember<int>("MusicStringLockedIndex");
  MusicTrackNameSize = EnsureGetMember<float>("MusicTrackNameSize");
  MusicTrackNameOffsetX = EnsureGetMember<int>("MusicTrackNameOffsetX");
  MusicTrackArtistSize = EnsureGetMember<int>("MusicTrackArtistSize");
  MusicTrackArtistOffsetX = EnsureGetMember<int>("MusicTrackArtistOffsetX");
  MusicTrackNumberOffsetX = EnsureGetMember<int>("MusicTrackNumberOffsetX");
  MusicButtonTextYOffset = EnsureGetMember<int>("MusicButtonTextYOffset");
  MusicButtonTextColor = EnsureGetMember<uint32_t>("MusicButtonTextColor");
  MusicButtonTextOutlineColor =
      EnsureGetMember<uint32_t>("MusicButtonTextOutlineColor");
  MusicButtonPlayingDispOffset =
      EnsureGetMember<glm::vec2>("MusicButtonPlayingDispOffset");
  MusicButtonBounds = EnsureGetMember<RectF>("MusicButtonBounds");
  MusicDirectionalHoldTime = EnsureGetMember<float>("MusicDirectionalHoldTime");
  MusicDirectionalFocusTimeInterval =
      EnsureGetMember<float>("MusicDirectionalFocusTimeInterval");
  MusicButtonHoverSprite = EnsureGetMember<Sprite>("MusicButtonHoverSprite");
  MusicButtonSelectSprite = EnsureGetMember<Sprite>("MusicButtonSelectSprite");
  MusicButtonPlayingSprite =
      EnsureGetMember<Sprite>("MusicButtonPlayingSprite");
  MusicNowPlayingNotificationSprite =
      EnsureGetMember<Sprite>("MusicNowPlayingNotificationSprite");

  MusicNowPlayingNotificationPos =
      EnsureGetMember<glm::vec2>("MusicNowPlayingNotificationPos");
  MusicNowPlayingNotificationFadeIn =
      EnsureGetMember<float>("MusicNowPlayingNotificationFadeIn");
  MusicNowPlayingNotificationFadeOut =
      EnsureGetMember<float>("MusicNowPlayingNotificationFadeOut");
  MusicNowPlayingNotificationTrackOffset =
      EnsureGetMember<glm::vec2>("MusicNowPlayingNotificationTrackOffset");
  MusicNowPlayingNotificationTrackFontSize =
      EnsureGetMember<int>("MusicNowPlayingNotificationTrackFontSize");
  MusicNowPlayingTextColor =
      EnsureGetMember<uint32_t>("MusicNowPlayingTextColor");
  MusicNowPlayingTextOutlineColor =
      EnsureGetMember<uint32_t>("MusicNowPlayingTextOutlineColor");
  GetMemberArray<Sprite>(MusicPlayingModeSprites.data(),
                         MusicMenuPlayingMode::_size(),
                         "MusicPlayingModeSprites");
  GetMemberArray<RectF>(MusicPlayingModeDisplayBounds.data(),
                        MusicMenuPlayingMode::_size(),
                        "MusicPlayingModeDisplayBounds");

  MovieDiskSprites = GetMemberVector<Sprite>("MovieDiskSprites");
  MovieDiskHighlightSprites =
      GetMemberVector<Sprite>("MovieDiskHighlightSprites");
  MovieDiskDisplayPositions =
      GetMemberVector<glm::vec2>("MovieDiskDisplayPositions");
  SubMenuFadeInDuration = EnsureGetMember<float>("SubMenuFadeInDuration");
  SubMenuFadeOutDuration = EnsureGetMember<float>("SubMenuFadeOutDuration");
  MovieDiskPlayIds = GetMemberVector<int>("MovieDiskPlayIds");

  AlbumMenuGuideSprite = EnsureGetMember<Sprite>("AlbumMenuGuideSprite");
  MusicMenuGuideSprite = EnsureGetMember<Sprite>("MusicMenuGuideSprite");
  MovieMenuGuideSprite = EnsureGetMember<Sprite>("MovieMenuGuideSprite");

  UI::LibraryMenuPtr = new UI::CCLCC::LibraryMenu();
  UI::Menus[drawType].push_back(UI::LibraryMenuPtr);
  // Don't push library submenus to the main menus list, let library menu handle
  // it
  UI::AlbumMenuPtr = new UI::CCLCC::AlbumMenu();
  UI::MusicMenuPtr = new UI::CCLCC::MusicMenu();
  UI::MovieMenuPtr = new UI::CCLCC::MovieMenu();
}

}  // namespace LibraryMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto