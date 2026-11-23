#include "titlemenu_switch.h"

#include "../../profile/scriptvars.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/chlcc/titlemenu.h"
#include "../../background2d.h"

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CHLCC::TitleMenu;
using namespace Impacto::UI::Widgets::CHLCC;
using namespace Impacto::UI::CHLCC::TitleMenuMode::Switch;

namespace Impacto {
namespace UI {
namespace CHLCC_Switch {

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  ScrWork[SW_TITLECUR] = target->Id;
  SetFlag(SF_TITLEEND, true);
  ChoiceMade = true;
  //  disable focus immediately, so MainItems couldn't get stuck being hovered
  MainItems->HasFocus = false;
}

TitleMenu::TitleMenu() : CHLCC::TitleMenu() {
  Load->OnClickHandler = [this](Widgets::Button* target) {
    ContinueButtonOnClick(target);
  };
  Extra->OnClickHandler = [this](Widgets::Button* target) {
    ExtraButtonOnClick(target);
  };

  const ButtonClickHandler switchOnClick = [this](auto* btn) {
    MenuButtonOnClick(btn);
  };
  CreateLoadItems(switchOnClick);
  CreateLockedExtraItems(switchOnClick);
  CreateUnlockedExtraItems(switchOnClick);
  CurrentExtraItems = LockedExtraItems;
}

void TitleMenu::ContinueButtonOnClick(Widgets::Button* target) {
  MenuButtonOnClick(target);
}

void TitleMenu::ExtraButtonOnClick(Widgets::Button* target) {
  MenuButtonOnClick(target);
}

void TitleMenu::Update(float dt) {
  const auto presentationState = static_cast<Mode>(ScrWork[SW_TITLEMODE]);
  UpdateInput(dt);
  PressToStartAnimation.Update(dt);
  SpinningCircleAnimation.Update(dt);
  SpinningCircleFlashingAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else if (State == Shown
             // && presentationState != ExtraSubEntriesControl
  ) {
    // when loading/starting a game from a submenu
    Hide();
  }

  // CHLCC has "congrats" screen that is rendered as a BG on top of the title
  // menu while it's still focused, and it's possible to hover extraSubItems
  const bool hasVisibleBGs =
      std::ranges::any_of(Impacto::Backgrounds2D,
                          [](const auto& pair) { return pair.second->Show; });
  if (State == Shown && IsFocused && !hasVisibleBGs) {
    MainItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
    MainItems->UpdateInput(dt);
    MainItems->Update(dt);
    const float secondarySmoothProgress =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    LoadItems->Tint.a = secondarySmoothProgress;
    LoadItems->UpdateInput(dt);
    LoadItems->Update(dt);
    CurrentExtraItems->Tint.a = secondarySmoothProgress;
    CurrentExtraItems->UpdateInput(dt);
    CurrentExtraItems->Update(dt);
    SystemItems->Tint.a = secondarySmoothProgress;
    SystemItems->UpdateInput(dt);
    SystemItems->Update(dt);

    switch (presentationState) {
      case IntroAnimation: {
        if (IntroSequence.IntroAnimation.IsIn() && ScrWork[SW_TITLECT] == 0) {
          ResetIntroSequence();
        }
        if (!IntroSequence.IntroAnimation.IsPlaying()) {
          IntroSequence.IntroAnimation.StartIn();
        }

        // Skip the animation if requested
        if (ScrWork[SW_TITLECT] >= 934 &&
            IntroSequence.IntroAnimation.State == AnimationState::Playing) {
          IntroSequence.IntroAnimation.Finish();
        }

        if (!IntroSequence.SeiraAnimation.IsOut() &&
            SpinningCircleAnimation.State == AnimationState::Stopped) {
          SpinningCircleAnimation.StartIn();
          SpinningCircleFlashingAnimation.StartIn();
        }

        IntroSequence.Update(dt);

        MainItems->Hide();
        // When returning to title menu from loading a game we need to hide the
        // load sub-menu
        if (LoadItems->VisibilityState != Hidden) {
          SecondaryFadeAnimation.StartOut();
          MainItems->HasFocus = true;
          LoadItems->Hide();
        }
      } break;
      case PressStart: {
        MainItems->MoveTo({-ItemHighlightOffset.x, 0});
        if (!GetFlag(SF_TITLEMODE)) {
          Hide();
          break;
        }
        if (PressToStartAnimation.State == AnimationState::Stopped) {
          PressToStartAnimation.StartIn();
        }
      } break;
      case EmptyBackground: {
        MainItems->MoveTo({-ItemHighlightOffset.x, 0});
        PrimaryFadeAnimation.Reset(AnimationDirection::In);
        SecondaryFadeAnimation.Reset(AnimationDirection::In);
      } break;
      case MainEntriesFading: {
        if (MainItems->VisibilityState == Hidden && ScrWork[SW_TITLECT] == 0) {
          MainItems->Show();
          MainItems->Tint.a = 0.0f;
          CurrentlyFocusedElement = Start;
          Start->HasFocus = true;
          PrimaryFadeAnimation.StartIn();
          const glm::vec2 mainItemsEndPos = {0, 0};
          MainItems->MoveTo(mainItemsEndPos, PrimaryFadeAnimation.DurationIn);
        } else if (MainItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          PrimaryFadeAnimation.StartOut();
          const glm::vec2 mainItemsEndPos = {-ItemHighlightOffset.x, 0};
          MainItems->MoveTo(mainItemsEndPos, PrimaryFadeAnimation.DurationOut);

        } else if (ScrWork[SW_TITLECT] == 0) {
          MainItems->Hide();
        }

      } break;
      case MainEntriesControl: {
        MainItems->MoveTo({0, 0});
        if (!GetFlag(SF_TITLEMODE)) {
          Hide();
        }
      } break;
      // case LoadSubEntriesFading: {
      //   if (LoadItems->VisibilityState == Hidden && ScrWork[SW_TITLECT] == 0)
      //   {
      //     LoadItems->Show();
      //     LoadItems->Tint.a = 0.0f;
      //     MainItems->HasFocus = false;
      //     CurrentlyFocusedElement = QuickLoad;
      //     QuickLoad->HasFocus = true;
      //     SecondaryFadeAnimation.StartIn();

      //  } else if (LoadItems->VisibilityState != Hidden &&
      //             ScrWork[SW_TITLECT] == 32) {
      //    SecondaryFadeAnimation.StartOut();

      //  } else if (ScrWork[SW_TITLECT] == 0) {
      //    LoadItems->Hide();
      //    MainItems->HasFocus = true;
      //  }
      //} break;
      // case ExtraSubEntriesFading: {
      //  if (CurrentExtraItems->VisibilityState == Hidden &&
      //      ScrWork[SW_TITLECT] == 0) {
      //    CurrentExtraItems->Show();
      //    CurrentExtraItems->Tint.a = 0.0f;
      //    MainItems->HasFocus = false;
      //    CurrentlyFocusedElement = ClearList;
      //    ClearList->HasFocus = true;
      //    SecondaryFadeAnimation.StartIn();

      //  } else if (CurrentExtraItems->VisibilityState != Hidden &&
      //             ScrWork[SW_TITLECT] == 32) {
      //    SecondaryFadeAnimation.StartOut();

      //  } else if (ScrWork[SW_TITLECT] == 0) {
      //    CurrentExtraItems->Hide();
      //    MainItems->HasFocus = true;
      //  }
      //} break;
      // case SystemSubEntriesFading: {
      //  if (SystemItems->VisibilityState == Hidden &&
      //      ScrWork[SW_TITLECT] == 0) {
      //    SystemItems->Show();
      //    SystemItems->Tint.a = 0.0f;
      //    MainItems->HasFocus = false;
      //    CurrentlyFocusedElement = Config;
      //    Config->HasFocus = true;
      //    SecondaryFadeAnimation.StartIn();

      //  } else if (SystemItems->VisibilityState != Hidden &&
      //             ScrWork[SW_TITLECT] == 32) {
      //    SecondaryFadeAnimation.StartOut();

      //  } else if (ScrWork[SW_TITLECT] == 0) {
      //    SystemItems->Hide();
      //    MainItems->HasFocus = true;
      //  }
      //} break;
      default: {
        ImpLog(LogLevel::Error, LogChannel::General,
               "Unknown title menu state: {}\n", presentationState);

        break;
      }
    }
  }
}

void TitleMenu::Render() {
  const auto presentationState = static_cast<Mode>(ScrWork[SW_TITLEMODE]);
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    if (ScrWork[SW_MENUCT] < 64) {
      switch (presentationState) {
        case IntroAnimation: {
          if (IntroSequence.FallingStarsAnimation.IsIn()) {
            Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
          }

          if (SpinningCircleAnimation.State == AnimationState::Playing) {
            DrawSpinningCircle(IntroSequence.SeiraAnimation.Progress);
          }

          IntroSequence.Render();
        } break;
        case PressStart: {
          DrawTitleMenuBackGraphics();
          glm::vec4 col = glm::vec4(1.0f);
          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
          Renderer->DrawSprite(PressToStartSprite, PressToStartPos, col);
        } break;
        case EmptyBackground: {
          DrawTitleMenuBackGraphics();
        } break;
        case MainEntriesFading: {
          DrawTitleMenuBackGraphics();
          MainItems->Render();
        } break;
        case MainEntriesControl: {
          DrawTitleMenuBackGraphics();
          MainItems->Render();
        } break;
        /*case LoadSubEntriesFading:
        case LoadSubEntriesControl: {
          DrawTitleMenuBackGraphics();
          LoadItems->Render();
          MainItems->Render();
        } break;
        case ExtraSubEntriesFading:
          [[fallthrough]];
        case ExtraSubEntriesControl: {
          DrawTitleMenuBackGraphics();
          CurrentExtraItems->Render();
          MainItems->Render();
        } break;
        case SystemSubEntriesFading:
          [[fallthrough]];
        case SystemSubEntriesControl: {
          DrawTitleMenuBackGraphics();
          SystemItems->Render();
          MainItems->Render();
        } break;*/
        default: {
          ImpLog(LogLevel::Error, LogChannel::General,
                 "Unknown title menu state: {}\n", presentationState);
        } break;
      }
    }

    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
    Renderer->DrawQuad(RectF{0.0f, 0.0f, Profile::Game::DesignWidth,
                             Profile::Game::DesignHeight},
                       col);
  }
}

}  // namespace CHLCC_Switch
}  // namespace UI
}  // namespace Impacto
