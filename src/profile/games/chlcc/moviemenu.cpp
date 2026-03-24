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
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");
  SelectedMovieAnimation =
      EnsureGetMember<SpriteAnimationDef>("SelectedMovieAnimation");
  SelectedMovieYellowDot = EnsureGetMember<Sprite>("SelectedMovieYellowDot");

  SelectMovie = EnsureGetMember<std::vector<Sprite>>("SelectMovie");
  SelectMoviePos = EnsureGetMember<std::vector<glm::vec2>>("SelectMoviePos");
  if (SelectMovie.size() != SelectMoviePos.size()) {
    throw std::runtime_error("Related arrays have mismatching sizes");
  }
  LabelPosition = EnsureGetMember<glm::vec2>("LabelPosition");
  MovieLabel = EnsureGetMember<Sprite>("MovieLabel");
  ListPosition = EnsureGetMember<glm::vec2>("ListPosition");
  MovieList = EnsureGetMember<Sprite>("MovieList");
  GetMemberArray<glm::vec2>(std::span(BoxPositions, Movies), "BoxPositions");
  MovieBox = EnsureGetMember<Sprite>("MovieBox");
  GetMemberArray<glm::vec2>(std::span(ThumbnailPositions, Movies),
                            "ThumbnailPositions");
  GetMemberArray<Sprite>(std::span(MoviesThumbnails, Movies),
                         "MoviesThumbnails");
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

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::Menus[drawType].push_back(new UI::CHLCC::MovieMenu());
}

}  // namespace MovieMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto