#include "titlemenu_ps3.h"

#include "../../profile/scriptvars.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/chlcc/titlemenu.h"
#include "../../background2d.h"

#include <algorithm>

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CHLCC::TitleMenu;
using namespace Impacto::UI::Widgets::CHLCC;
using namespace Impacto::UI::CHLCC::TitleMenuMode::PS3;

namespace Impacto {
namespace UI {
namespace CHLCC_PS3 {

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  ScrWork[SW_TITLECUR1] = target->Id;
  ChoiceMade = true;
  //  disable focus immediately, so MainItems couldn't get stuck being hovered
  MainItems->HasFocus = false;
}

void TitleMenu::SecondaryButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR2] = target->Id;
  ChoiceMade = true;
}

TitleMenu::TitleMenu() : CHLCC::TitleMenu() {
  const ButtonClickHandler secondaryOnClick = [this](auto* btn) {
    SecondaryButtonOnClick(btn);
  };
  CreateLoadItems(secondaryOnClick);
  CreateLockedExtraItems(secondaryOnClick);
  CreateUnlockedExtraItems(secondaryOnClick);

  if (ConfigButtonId.has_value() && SystemSaveButtonId.has_value()) {
    Config = new TitleButton(
        *ConfigButtonId, MenuEntriesSprites[12], MenuEntriesHSprites[12],
        SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemConfigY));
    Config->OnClickHandler = secondaryOnClick;
    Config->IsSubButton = true;
    Config->LineDecoration = LineSprites[2];
    Config->LineY = SecondaryMenuSystemConfigY;
    SystemItems->Add(Config, FDIR_DOWN);

    SystemSave =
        new TitleButton(*SystemSaveButtonId, MenuEntriesSprites[13],
                        MenuEntriesHSprites[13], SecondaryItemHighlightSprite,
                        glm::vec2(SecondaryItemX, ItemSystemSaveY));
    SystemSave->OnClickHandler = secondaryOnClick;
    SystemSave->IsSubButton = true;
    SystemSave->LineDecoration = LineSprites[3];
    SystemSave->LineY = SecondaryMenuSystemSaveY;
    SystemItems->Add(SystemSave, FDIR_DOWN);
  }

  CurrentExtraItems = LockedExtraItems;
}

void TitleMenu::Show() {
  if (State != Shown) {
    CurrentExtraItems =
        GetFlag(SF_EXTRA_ENA) ? UnlockedExtraItems : LockedExtraItems;
    ClearList = static_cast<TitleButton*>(CurrentExtraItems->Children[0]);
    State = Shown;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    if (PressToStartAnimation.State == AnimationState::Stopped) {
      PressToStartAnimation.StartIn();
    }

    if (SpinningCircleAnimation.State == AnimationState::Stopped) {
      SpinningCircleAnimation.StartIn();
      SpinningCircleFlashingAnimation.StartIn();
    }
  }
}

void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hidden;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;

    MainItems->Hide();
    LoadItems->Hide();
    SystemItems->Hide();
    UnlockedExtraItems->Hide();
  }
}

void TitleMenu::Update(float dt) {
  const auto presentationState = static_cast<Mode>(ScrWork[SW_TITLEDISPCT]);
  UpdateInput(dt);
  PressToStartAnimation.Update(dt);
  SpinningCircleAnimation.Update(dt);
  SpinningCircleFlashingAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else if (State == Shown && presentationState != ExtraSubEntriesControl) {
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
      case LoadSubEntriesFading: {
        if (LoadItems->VisibilityState == Hidden && ScrWork[SW_TITLECT] == 0) {
          LoadItems->Show();
          LoadItems->Tint.a = 0.0f;
          MainItems->HasFocus = false;
          CurrentlyFocusedElement = QuickLoad;
          QuickLoad->HasFocus = true;
          SecondaryFadeAnimation.StartIn();

        } else if (LoadItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

        } else if (ScrWork[SW_TITLECT] == 0) {
          LoadItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case ExtraSubEntriesFading: {
        if (CurrentExtraItems->VisibilityState == Hidden &&
            ScrWork[SW_TITLECT] == 0) {
          CurrentExtraItems->Show();
          CurrentExtraItems->Tint.a = 0.0f;
          MainItems->HasFocus = false;
          CurrentlyFocusedElement = ClearList;
          ClearList->HasFocus = true;
          SecondaryFadeAnimation.StartIn();

        } else if (CurrentExtraItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

        } else if (ScrWork[SW_TITLECT] == 0) {
          CurrentExtraItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case SystemSubEntriesFading: {
        if (SystemItems->VisibilityState == Hidden &&
            ScrWork[SW_TITLECT] == 0) {
          SystemItems->Show();
          SystemItems->Tint.a = 0.0f;
          MainItems->HasFocus = false;
          CurrentlyFocusedElement = Config;
          Config->HasFocus = true;
          SecondaryFadeAnimation.StartIn();

        } else if (SystemItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

        } else if (ScrWork[SW_TITLECT] == 0) {
          SystemItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      default:
        break;
    }
  }
}

void TitleMenu::Render() {
  const auto presentationState = static_cast<Mode>(ScrWork[SW_TITLEDISPCT]);
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
        case LoadSubEntriesFading:
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
        } break;
        default:
          break;
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

}  // namespace CHLCC_PS3
}  // namespace UI
}  // namespace Impacto
