#include "moviemenu.h"

#include "../../profile/games/chlcc/moviemenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
using namespace Impacto::Profile::CHLCC::MovieMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::CHLCC;

void MovieMenu::MovieButtonOnClick(Widgets::Button* target) {
  auto movieButton = static_cast<MovieMenuEntryButton*>(target);
  if (!movieButton->IsLocked) {
    switch (movieButton->Id) {
      case 0: {
        ScrWork[SW_MOVIEMODE_CUR] = 10;
      } break;
      case 4:
      case 5:
      case 6: {
        ScrWork[SW_MOVIEMODE_CUR] = movieButton->Id;
      } break;
      case 7: {
        ScrWork[SW_MOVIEMODE_CUR] = 3;
      } break;
      default: {
        ScrWork[SW_MOVIEMODE_CUR] = movieButton->Id - 1;
      }
    }
  }
}

MovieMenu::MovieMenu() {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.SetDuration(MenuTransitionDuration);

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  SelectMovieTextFade.Direction = AnimationDirection::In;
  SelectMovieTextFade.LoopMode = AnimationLoopMode::Loop;
  SelectMovieTextFade.DurationIn = SelectMovieFadeDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  auto onClick =
      std::bind(&MovieMenu::MovieButtonOnClick, this, std::placeholders::_1);

  // Movie Buttons initialization
  MovieItems = new Widgets::Group(this);
  MovieItems->WrapFocus = false;

  for (int i = 0; i < 10; i++) {
    glm::vec2 thumbnailPosition(ThumbnailPositions[i].x,
                                ThumbnailPositions[i].y);
    glm::vec2 boxPosition(BoxPositions[i].x, BoxPositions[i].y);
    MovieMenuEntryButton* movieMenuEntryButton =
        new MovieMenuEntryButton(i, MoviesThumbnails[i], LockedThumbnail,
                                 thumbnailPosition, boxPosition);
    movieMenuEntryButton->OnClickHandler = onClick;
    MovieItems->Add(movieMenuEntryButton);
  }
}

void MovieMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      SelectMovieTextFade.StartIn();
    }
    MovieItems->Show();
    State = Showing;
    UpdateMovieEntries();
    ScrWork[SW_MOVIEMODE_CUR] = 255;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}

void MovieMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
      SelectMovieTextFade.StartOut();
    }
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void MovieMenu::Render() {
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawRect(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();
    DrawRedBar();

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);
      Renderer->DrawSprite(MenuTitleText, RightTitlePos, glm::vec4(1.0f),
                           glm::vec2(1.0f), MenuTitleTextAngle);
      Renderer->DrawSprite(MenuTitleText, LeftTitlePos);
    }

    glm::vec3 tint = {1.0f, 1.0f, 1.0f};
    // Alpha goes from 0 to 1 in half the time
    float alpha =
        MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
    Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         glm::vec4(tint, alpha));

    float yOffset = 0;
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.73f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -720.0f, 0.0f,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }

      MovieItems->MoveTo(glm::vec2(0, yOffset));
      MovieItems->Render();
      glm::vec2 listPosition(ListPosition.x, ListPosition.y + yOffset);
      Renderer->DrawSprite(MovieList, listPosition);
      DrawButtonPrompt();
      DrawSelectMovie(yOffset);
    }
  }
}

void MovieMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MovieItems->UpdateInput();
  }
}

void MovieMenu::Update(float dt) {
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_MOVIEMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && State == Hiding) {
    State = Hidden;
    MovieItems->Hide();
  } else if (MenuTransition.IsIn() && State == Showing) {
    State = Shown;
    MovieItems->Show();
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    SelectMovieTextFade.Update(dt);
    if (MenuTransition.Direction == -1.0f && MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == 1.0f || TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    MovieItems->Update(dt);
    UpdateTitles();
  }
}

inline void MovieMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        scale *= CircleSprite.Bounds.Height / 106.0f;
        Renderer->DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

inline void MovieMenu::DrawErin() {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = 801.0f;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void MovieMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}

inline void MovieMenu::DrawButtonPrompt() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (MenuTransition.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (MenuTransition.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
  }
}

inline void MovieMenu::DrawSelectMovie(float yOffset) {
  float alpha;
  for (int idx = 0; idx < 11; idx++) {
    alpha = 1.0f;
    if (SelectMovieTextFade.Progress < 0.046f * (idx + 1)) {
      alpha = (SelectMovieTextFade.Progress - 0.046f * idx) / 0.046f;
    }
    Renderer->DrawSprite(
        SelectMovie[idx],
        glm::vec2(SelectMoviePos[idx].x, SelectMoviePos[idx].y + yOffset),
        glm::vec4(glm::vec3(1.0f), alpha));
  }
}

void MovieMenu::UpdateTitles() {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MenuTitleTextRightPosition;
  LeftTitlePos = glm::vec2(
      MenuTitleTextLeftPosition.x,
      TitleFade.IsIn()
          ? MenuTitleTextLeftPosition.y
          : glm::mix(
                1.0f, 721.0f,
                1.01011f * std::sin(1.62223f * TitleFade.Progress + 3.152f) +
                    1.01012f));

  if (MenuTransition.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
}

void MovieMenu::UpdateMovieEntries() {
  for (auto el : MovieItems->Children) {
    auto movieButton = static_cast<Widgets::CHLCC::MovieMenuEntryButton*>(el);
    if (movieButton->Id == 0 || movieButton->Id == 1)
      movieButton->IsLocked = false;
    else
      movieButton->IsLocked = !((GetFlag(SF_MOVIE_UNLOCK1)) ||
                                (GetFlag(SF_CLR_END1 + movieButton->Id)));
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto