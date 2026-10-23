#include "librarymenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/librarymenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace LibraryMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  GetMemberArray<Sprite>(std::span(TabSprites, TabCount), "TabSprites");
  GetMemberArray<Sprite>(std::span(TabSelectedSprites, TabCount),
                         "TabSelectedSprites");
  TabPos = EnsureGetMember<glm::vec2>("TabPos");
  TabPitch = EnsureGetMember<float>("TabPitch");
  GetMemberArray<Sprite>(std::span(TitleSprites, LibraryCount), "TitleSprites");
  TitlePos = EnsureGetMember<glm::vec2>("TitlePos");
  GetMemberArray<Sprite>(std::span(GuideSprites, GuideCount), "GuideSprites");
  GuideRight = EnsureGetMember<float>("GuideRight");
  GuideY = EnsureGetMember<float>("GuideY");

  ThumbnailSprites = EnsureGetMember<std::vector<Sprite>>("ThumbnailSprites");
  NoDataSprite = EnsureGetMember<Sprite>("NoDataSprite");
  NoDataSelectedSprite = EnsureGetMember<Sprite>("NoDataSelectedSprite");
  SelectedFrameSprite = EnsureGetMember<Sprite>("SelectedFrameSprite");
  SelectedGlowSprite = EnsureGetMember<Sprite>("SelectedGlowSprite");
  CgGridPos = EnsureGetMember<glm::vec2>("CgGridPos");
  CgCellPitch = EnsureGetMember<glm::vec2>("CgCellPitch");
  CgColumns = EnsureGetMember<int>("CgColumns");
  CgRows = EnsureGetMember<int>("CgRows");
  ThumbnailOffset = EnsureGetMember<glm::vec2>("ThumbnailOffset");
  ScrollThumbSprite = EnsureGetMember<Sprite>("ScrollThumbSprite");
  ScrollX = EnsureGetMember<float>("ScrollX");
  ScrollTop = EnsureGetMember<float>("ScrollTop");
  ScrollBottom = EnsureGetMember<float>("ScrollBottom");
  AllClearCgs = EnsureGetMember<std::vector<int>>("AllClearCgs");
  EndingFlags = EnsureGetMember<std::vector<int>>("EndingFlags");
  CgLoadBuffer = EnsureGetMember<int>("CgLoadBuffer");
  CgZoomMax = EnsureGetMember<float>("CgZoomMax");
  CgScrollSpeed = EnsureGetMember<float>("CgScrollSpeed");

  PlayerSprite = EnsureGetMember<Sprite>("PlayerSprite");
  PlayerPos = EnsureGetMember<glm::vec2>("PlayerPos");
  ListHeaderSprite = EnsureGetMember<Sprite>("ListHeaderSprite");
  GetMemberArray<Sprite>(std::span(ListRowSprites, 2), "ListRowSprites");
  ListSelectedSprite = EnsureGetMember<Sprite>("ListSelectedSprite");
  ListBounds = EnsureGetMember<RectF>("ListBounds");
  ListRowHeight = EnsureGetMember<float>("ListRowHeight");
  ListVisibleRows = EnsureGetMember<int>("ListVisibleRows");
  ListTrackX = EnsureGetMember<float>("ListTrackX");
  ListTitleX = EnsureGetMember<float>("ListTitleX");
  ListArtistX = EnsureGetMember<float>("ListArtistX");
  ListFontSize = EnsureGetMember<float>("ListFontSize");
  NowPlayingPos = EnsureGetMember<glm::vec2>("NowPlayingPos");
  NowPlayingFontSize = EnsureGetMember<float>("NowPlayingFontSize");
  GetMemberArray<Sprite>(std::span(RepeatModeSprites, 4), "RepeatModeSprites");
  RepeatModePos = EnsureGetMember<glm::vec2>("RepeatModePos");
  PlayIconSprite = EnsureGetMember<Sprite>("PlayIconSprite");
  TrackStringBase = EnsureGetMember<int>("TrackStringBase");
  LockedTrackString = EnsureGetMember<int>("LockedTrackString");
  TrackBgmIds = EnsureGetMember<std::vector<int>>("TrackBgmIds");
  TrackBgmAltIds = EnsureGetMember<std::vector<int>>("TrackBgmAltIds");
  TrackMovieUnlocks = EnsureGetMember<std::vector<int>>("TrackMovieUnlocks");

  MovieThumbnailSprites =
      EnsureGetMember<std::vector<Sprite>>("MovieThumbnailSprites");
  MovieIds = EnsureGetMember<std::vector<int>>("MovieIds");
  MovieGridPos = EnsureGetMember<glm::vec2>("MovieGridPos");
  MovieCellPitch = EnsureGetMember<glm::vec2>("MovieCellPitch");
  MovieColumns = EnsureGetMember<int>("MovieColumns");
  MovieThumbnailScale = EnsureGetMember<float>("MovieThumbnailScale");
  MovieTitleOffsetY = EnsureGetMember<float>("MovieTitleOffsetY");
  MovieStringBase = EnsureGetMember<int>("MovieStringBase");
  MovieFontSize = EnsureGetMember<float>("MovieFontSize");
  MovieUnlockLines =
      EnsureGetMember<std::vector<std::vector<int>>>("MovieUnlockLines");

  TextColor = EnsureGetMember<uint32_t>("TextColor");
  LockedTextColor = EnsureGetMember<uint32_t>("LockedTextColor");
  SelectedTextColor = EnsureGetMember<uint32_t>("SelectedTextColor");
  SystemScriptBuffer = EnsureGetMember<int>("SystemScriptBuffer");
  CountMax = EnsureGetMember<int>("CountMax");

  UI::LibraryMenuPtr = new UI::SG0::LibraryMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::LibraryMenuPtr);
}

}  // namespace LibraryMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
