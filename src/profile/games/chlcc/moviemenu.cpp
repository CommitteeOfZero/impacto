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
  MenuTransitionDuration = EnsureGetMember<float>("TransitionDuration");
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");
  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");
  SelectedMovieAnimation =
      EnsureGetMember<SpriteAnimationDef>("SelectedMovieAnimation");
  SelectedMovieYellowDot = EnsureGetMember<Sprite>("SelectedMovieYellowDot");

  SelectMovieFadeDuration = EnsureGetMember<float>("SelectMovieFadeDuration");
  GetMemberArray<Sprite>(SelectMovie, 11, "SelectMovie");
  GetMemberArray<glm::vec2>(SelectMoviePos, 11, "SelectMoviePos");
  LabelPosition = EnsureGetMember<glm::vec2>("LabelPosition");
  MovieLabel = EnsureGetMember<Sprite>("MovieLabel");
  ListPosition = EnsureGetMember<glm::vec2>("ListPosition");
  MovieList = EnsureGetMember<Sprite>("MovieList");
  GetMemberArray<glm::vec2>(BoxPositions, Movies, "BoxPositions");
  MovieBox = EnsureGetMember<Sprite>("MovieBox");
  GetMemberArray<glm::vec2>(ThumbnailPositions, Movies, "ThumbnailPositions");
  GetMemberArray<Sprite>(MoviesThumbnails, Movies, "MoviesThumbnails");
  LockedThumbnail = EnsureGetMember<Sprite>("LockedThumbnail");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPromptSprite");

  MovieExtraVideosEnabled =
      TryGetMember<bool>("MovieExtraVideosEnabled").value_or(false);
  if (MovieExtraVideosEnabled) {
    SelectedMovieExtraAnimation =
        EnsureGetMember<SpriteAnimationDef>("SelectedMovieExtraAnimation");
    MovieBoxExtra = EnsureGetMember<Sprite>("MovieBoxExtra");
    MovieThumbnailExtraOp = EnsureGetMember<Sprite>("MovieThumbnailExtraOp");
    MovieButtonExtraPromptPosition =
        EnsureGetMember<glm::vec2>("MovieButtonExtraPromptPosition");
    MovieButtonExtraPrompt = EnsureGetMember<Sprite>("MovieButtonExtraPrompt");
  }

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::Menus[drawType].push_back(new UI::CHLCC::MovieMenu());
}

}  // namespace MovieMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto