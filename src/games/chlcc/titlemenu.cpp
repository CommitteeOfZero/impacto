#include "titlemenu.h"

#include "../../spritesheet.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/chlcc/titlebutton.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/chlcc/titlemenu.h"
#include "../../profile/scriptvars.h"
#include "../../background2d.h"
#include "../../profile/game.h"
#include "../../profile/vm.h"
#include "../../profile/patch.h"
#include <vector>

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CHLCC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets::CHLCC;

// enum class TitleDispCtState : uint8_t {
//   IntroAnimation,
//   PressStart,
//   EmptyBackground,
//   MainEntriesFading,
//   MainEntriesControl,
//   LoadSubEntriesFading,
//   LoadSubEntriesControl,
//   ExtraSubEntriesFading,
//   ExtraSubEntriesControl,
//   SystemSubEntriesFading,
//   SystemSubEntriesControl,
// };

// static TitleDispCtState ResolveTitleDispCtState() {
//   if (Profile::Vm::GameInstructionSet ==
//       Impacto::Vm::InstructionSet::LCCSwitch) {
//     using namespace TitleMenuMode::Switch;
//     switch (ScrWork[SW_TITLEMODE]) {
//       case Mode::IntroAnimation:
//         return TitleDispCtState::IntroAnimation;
//       case Mode::PressStart:
//         return TitleDispCtState::PressStart;
//       case Mode::EmptyBackground:
//         return TitleDispCtState::EmptyBackground;
//       case Mode::MainEntriesFading:
//         return TitleDispCtState::MainEntriesFading;
//       case Mode::MainEntriesControl:
//         return TitleDispCtState::MainEntriesControl;
//     }
//   } else {
//     using namespace TitleMenuMode::PS3;
//     switch (static_cast<Mode>(ScrWork[SW_TITLEDISPCT])) {
//       case Mode::IntroAnimation:
//         return TitleDispCtState::IntroAnimation;
//       case Mode::PressStart:
//         return TitleDispCtState::PressStart;
//       case Mode::EmptyBackground:
//         return TitleDispCtState::EmptyBackground;
//       case Mode::MainEntriesFading:
//         return TitleDispCtState::MainEntriesFading;
//       case Mode::MainEntriesControl:
//         return TitleDispCtState::MainEntriesControl;
//       case Mode::LoadSubEntriesFading:
//         return TitleDispCtState::LoadSubEntriesFading;
//       case Mode::LoadSubEntriesControl:
//         return TitleDispCtState::LoadSubEntriesControl;
//       case Mode::ExtraSubEntriesFading:
//         return TitleDispCtState::ExtraSubEntriesFading;
//       case Mode::ExtraSubEntriesControl:
//         return TitleDispCtState::ExtraSubEntriesControl;
//       case Mode::SystemSubEntriesFading:
//         return TitleDispCtState::SystemSubEntriesFading;
//       case Mode::SystemSubEntriesControl:
//         return TitleDispCtState::SystemSubEntriesControl;
//     }
//   }
//   ImpLog(
//       LogLevel::Error, LogChannel::General, "Unknown title menu state: {}\n",
//       Profile::Vm::GameInstructionSet ==
//       Impacto::Vm::InstructionSet::LCCSwitch
//           ? ScrWork[SW_TITLEMODE]
//           : ScrWork[SW_TITLEDISPCT]);
//   return TitleDispCtState::EmptyBackground;
// }

void TitleMenu::ExitButtonOnClick(Widgets::Button* target) {
  PADinputButtonWentDown = PADinputButtonWentDown & ~PAD1A;
  PADinputMouseWentDown = PADinputMouseWentDown & ~PAD1A;
  Input::KeyboardButtonWentDown[SDL_SCANCODE_ESCAPE] = true;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  LoadItems = new Widgets::Group(this);
  LockedExtraItems = new Widgets::Group(this);
  UnlockedExtraItems = new Widgets::Group(this);
  SystemItems = new Widgets::Group(this);

  const ButtonClickHandler primaryOnClick = [this](auto* btn) {
    // virtual, depends on the realization
    MenuButtonOnClick(btn);
  };

  // Start menu button
  Start = new TitleButton(NewButtonId, MenuEntriesSprites[0],
                          MenuEntriesHSprites[0], ItemHighlightSprite,
                          glm::vec2(ItemHighlightOffset.x - 1.0f,
                                    ItemYBase - 1.0f + 0 * ItemPadding));
  Start->OnClickHandler = primaryOnClick;
  MainItems->Add(Start, FDIR_DOWN);

  // Load menu button
  Load = new TitleButton(PrimLoadButtonId, MenuEntriesSprites[1],
                         MenuEntriesHSprites[1], ItemHighlightSprite,
                         glm::vec2(ItemHighlightOffset.x - 1.0f,
                                   ItemYBase - 1.0f + 1 * ItemPadding));
  Load->OnClickHandler = primaryOnClick;
  MainItems->Add(Load, FDIR_DOWN);

  // Extra menu button
  Extra = new TitleButton(ExtraButtonId, MenuEntriesSprites[2],
                          MenuEntriesHSprites[2], ItemHighlightSprite,
                          glm::vec2(ItemHighlightOffset.x - 1.0f,
                                    ItemYBase - 1.0f + 2 * ItemPadding));
  Extra->OnClickHandler = primaryOnClick;
  MainItems->Add(Extra, FDIR_DOWN);

  // System menu button
  System = new TitleButton(SystemButtonId, MenuEntriesSprites[3],
                           MenuEntriesHSprites[3], ItemHighlightSprite,
                           glm::vec2(ItemHighlightOffset.x - 1.0f,
                                     ItemYBase - 1.0f + 3 * ItemPadding));
  System->OnClickHandler = primaryOnClick;
  MainItems->Add(System, FDIR_DOWN);

  // Exit menu button (Configuration/Patch driven)
  if (Patch::HasScriptedExitLogic && ExitButtonId.has_value()) {
    auto* const exitPtr = new TitleButton(
        *ExitButtonId, ExitSprite, ExitHighlightSprite, ItemHighlightSprite,
        glm::vec2(ItemHighlightOffset.x - 1.0f,
                  ItemYBase - 1.0f + 4 * ItemPadding));
    exitPtr->OnClickHandler = [this](auto* btn) {
      return ExitButtonOnClick(btn);
    };

    Exit.emplace(*exitPtr);
    MainItems->Add(exitPtr, FDIR_DOWN);
  }

  SpinningCircleAnimation.LoopMode = AnimationLoopMode::Loop;
  SpinningCircleAnimation.SetDuration(SpinningCircleAnimationDuration);
  SpinningCircleFlashingAnimation.LoopMode =
      AnimationLoopMode::ReverseDirection;
  SpinningCircleFlashingAnimation.SetDuration(
      SpinningCircleFlashingAnimationDuration);
}

void TitleMenu::CreateLoadItems(const ButtonClickHandler& onClick) {
  // Quick Load secondary Load menu button
  QuickLoad = new TitleButton(
      LoadButtonId, MenuEntriesSprites[4], MenuEntriesHSprites[4],
      SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemLoadQuickY));
  QuickLoad->OnClickHandler = onClick;
  QuickLoad->IsSubButton = true;
  QuickLoad->LineDecoration = LineSprites[2];
  QuickLoad->LineY = SecondaryMenuLoadLineY;
  LoadItems->Add(QuickLoad, FDIR_DOWN);

  // Sub Load secondary Load menu button
  SubLoad = new TitleButton(
      QLoadButtonId, MenuEntriesSprites[5], MenuEntriesHSprites[5],
      SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemLoadY));
  SubLoad->OnClickHandler = onClick;
  SubLoad->IsSubButton = true;
  SubLoad->LineDecoration = LineSprites[3];
  SubLoad->LineY = SecondaryMenuLoadQuickLineY;
  LoadItems->Add(SubLoad, FDIR_DOWN);
}

void TitleMenu::CreateLockedExtraItems(const ButtonClickHandler& onClick) {
  // Locked Extra Menus

  // Clear List secondary Extra menu button
  ClearList =
      new TitleButton(EndingListButtonId, MenuEntriesSprites[6],
                      MenuEntriesHSprites[6], SecondaryItemHighlightSprite,
                      glm::vec2(SecondaryItemX, ItemSoundLibraryY));
  ClearList->OnClickHandler = onClick;
  ClearList->IsSubButton = true;
  ClearList->LineDecoration = LineSprites[2];
  ClearList->LineY = SecondaryMenuExtraSoundY;
  LockedExtraItems->Add(ClearList, FDIR_DOWN);

  // Tips secondary Extra menu button
  Tips = new TitleButton(LockedTipsButtonId, MenuEntriesSprites[10],
                         MenuEntriesHSprites[10], SecondaryItemHighlightSprite,
                         glm::vec2(SecondaryItemX, ItemMovieLibraryY));
  Tips->OnClickHandler = onClick;
  Tips->IsSubButton = true;
  Tips->LineDecoration = LineSprites[3];
  Tips->LineY = SecondaryMenuExtraMovieY;
  LockedExtraItems->Add(Tips, FDIR_DOWN);

  // Trophy secondary Extra menu button

  if (LockedTrophyButtonId.has_value()) {
    Trophy = new TitleButton(
        *LockedTrophyButtonId, MenuEntriesSprites[11], MenuEntriesHSprites[11],
        SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemTipsY));
    Trophy->OnClickHandler = onClick;
    Trophy->IsSubButton = true;
    Trophy->LineDecoration = LineSprites[4];
    Trophy->LineY = SecondaryMenuExtraTipsY;
    LockedExtraItems->Add(Trophy, FDIR_DOWN);
  }
}

void TitleMenu::CreateUnlockedExtraItems(const ButtonClickHandler& onClick) {
  // Unlocked Extra Menus
  // Clear List secondary Extra menu button
  ClearList = new TitleButton(
      EndingListButtonId, MenuEntriesSprites[6], MenuEntriesHSprites[6],
      SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemClearListY));
  ClearList->OnClickHandler = onClick;
  ClearList->IsSubButton = true;
  ClearList->LineDecoration = LineSprites[0];
  ClearList->LineY = SecondaryMenuExtraClearY;
  UnlockedExtraItems->Add(ClearList, FDIR_DOWN);

  // CG Library secondary Extra menu button
  CGLibrary = new TitleButton(
      CGButtonId, MenuEntriesSprites[7], MenuEntriesHSprites[7],
      SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemCGLibraryY));
  CGLibrary->OnClickHandler = onClick;
  CGLibrary->IsSubButton = true;
  CGLibrary->LineDecoration = LineSprites[1];
  CGLibrary->LineY = SecondaryMenuExtraCGY;
  UnlockedExtraItems->Add(CGLibrary, FDIR_DOWN);

  // Sound Library secondary Extra menu button
  SoundLibrary =
      new TitleButton(SoundButtonId, MenuEntriesSprites[8],
                      MenuEntriesHSprites[8], SecondaryItemHighlightSprite,
                      glm::vec2(SecondaryItemX, ItemSoundLibraryY));
  SoundLibrary->OnClickHandler = onClick;
  SoundLibrary->IsSubButton = true;
  SoundLibrary->LineDecoration = LineSprites[2];
  SoundLibrary->LineY = SecondaryMenuExtraSoundY;
  UnlockedExtraItems->Add(SoundLibrary, FDIR_DOWN);

  // Movie Library secondary Extra menu button
  MovieLibrary =
      new TitleButton(MovieButtonId, MenuEntriesSprites[9],
                      MenuEntriesHSprites[9], SecondaryItemHighlightSprite,
                      glm::vec2(SecondaryItemX, ItemMovieLibraryY));
  MovieLibrary->OnClickHandler = onClick;
  MovieLibrary->IsSubButton = true;
  MovieLibrary->LineDecoration = LineSprites[3];
  MovieLibrary->LineY = SecondaryMenuExtraMovieY;
  UnlockedExtraItems->Add(MovieLibrary, FDIR_DOWN);

  // Tips secondary Extra menu button
  Tips = new TitleButton(TipsButtonId, MenuEntriesSprites[10],
                         MenuEntriesHSprites[10], SecondaryItemHighlightSprite,
                         glm::vec2(SecondaryItemX, ItemTipsY));
  Tips->OnClickHandler = onClick;
  Tips->IsSubButton = true;
  Tips->LineDecoration = LineSprites[4];
  Tips->LineY = SecondaryMenuExtraTipsY;
  UnlockedExtraItems->Add(Tips, FDIR_DOWN);

  // Trophy secondary Extra menu button
  if (TrophyButtonId.has_value()) {
    Trophy = new TitleButton(
        *TrophyButtonId, MenuEntriesSprites[11], MenuEntriesHSprites[11],
        SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemTrophyY));
    Trophy->OnClickHandler = onClick;
    Trophy->IsSubButton = true;
    Trophy->LineDecoration = LineSprites[5];
    Trophy->LineY = SecondaryMenuExtraTrophyY;
    UnlockedExtraItems->Add(Trophy, FDIR_DOWN);
  }
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

void TitleMenu::ResetIntroSequence() {
  IntroSequence.Reset();
  SpinningCircleAnimation.Reset();
  SpinningCircleFlashingAnimation.Reset();
}

void TitleMenu::DrawSpinningCircle(float alpha) const {
  const CornersQuad dest =
      SpinningCircleSprite.ScaledBounds()
          .Scale({2.0f, 2.0f}, {0.0f, 0.0f})
          .RotateAroundCenter(-SpinningCircleAnimation.Progress * 2.0f *
                              std::numbers::pi_v<float>)
          .Translate(SpinningCirclePosition);

  glm::vec4 tint = {1.0f, 1.0f, 1.0f, alpha};
  float intensity = SpinningCircleFlashingAnimation.Progress;
  glm::vec3 colorShift(intensity);

  Renderer->DrawSprite(SpinningCircleSprite, dest, tint, colorShift);
}

void TitleMenu::DrawTitleMenuBackGraphics() const {
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
  DrawSpinningCircle(1.0f);
  Renderer->DrawSprite(DelusionADVUnderSprite,
                       DelusionADVPosition - DelusionADVPopoutOffset);
  Renderer->DrawSprite(DelusionADVSprite, DelusionADVPosition);
  Renderer->DrawSprite(SeiraUnderSprite, SeiraUnderPosition);
  Renderer->DrawSprite(SeiraSprite, SeiraPosition);
  Renderer->DrawSprite(CHLogoSprite, CHLogoPosition);
  Renderer->DrawSprite(LCCLogoUnderSprite, LCCLogoUnderPosition);
  Renderer->DrawSprite(CopyrightTextSprite, CopyrightTextPosition);

  for (size_t i : LCCLogoDrawOrder) {
    Renderer->DrawSprite(LCCLogoSprites[i], LCCLogoPositions[i]);
  }

  Renderer->DrawSprite(StarLogoSprite, StarLogoPosition);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto
