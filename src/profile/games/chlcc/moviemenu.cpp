#include "moviemenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/chlcc/moviemenu.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace MovieMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMemberFloat("TransitionDuration");
  BackgroundColor = EnsureGetMemberUint("BackgroundColor");
  CircleSprite = EnsureGetMemberSprite("CircleSprite");
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
  MenuTitleTextRightPosition = EnsureGetMemberVec2("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition = EnsureGetMemberVec2("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMemberFloat("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMemberSprite("MenuTitleText");
  SelectedMovieAnimation = EnsureGetMemberAnimation("SelectedMovieAnimation");
  SelectedMovieYellowDot = EnsureGetMemberSprite("SelectedMovieYellowDot");

  SelectMovieFadeDuration = EnsureGetMemberFloat("SelectMovieFadeDuration");
  GetMemberSpriteArray(SelectMovie, 11, "SelectMovie");
  GetMemberVec2Array(SelectMoviePos, 11, "SelectMoviePos");
  LabelPosition = EnsureGetMemberVec2("LabelPosition");
  MovieLabel = EnsureGetMemberSprite("MovieLabel");
  ListPosition = EnsureGetMemberVec2("ListPosition");
  MovieList = EnsureGetMemberSprite("MovieList");
  GetMemberVec2Array(BoxPositions, Movies, "BoxPositions");
  MovieBox = EnsureGetMemberSprite("MovieBox");
  GetMemberVec2Array(ThumbnailPositions, Movies, "ThumbnailPositions");
  GetMemberSpriteArray(MoviesThumbnails, Movies, "MoviesThumbnails");
  LockedThumbnail = EnsureGetMemberSprite("LockedThumbnail");
  ButtonPromptPosition = EnsureGetMemberVec2("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMemberSprite("ButtonPromptSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::CHLCC::MovieMenu());
}

}  // namespace MovieMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto