#include "titlemenu.h"

#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../io/vfs.h"

#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/label.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/chlcc/titlemenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CHLCC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::CHLCC;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR1] = target->Id;
  ChoiceMade = true;
}

void TitleMenu::SecondaryButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR2] = target->Id;
  ChoiceMade = true;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  LoadItems = new Widgets::Group(this);
  ExtraItems = new Widgets::Group(this);
  SystemItems = new Widgets::Group(this);

  auto onClick =
      std::bind(&TitleMenu::MenuButtonOnClick, this, std::placeholders::_1);
  auto secondaryOnClick = std::bind(&TitleMenu::SecondaryButtonOnClick, this,
                                    std::placeholders::_1);

  // Start menu button
  Start = new TitleButton(
      0, MenuEntriesSprites[0], MenuEntriesHSprites[0], ItemHighlightSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) +
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (0 * ItemPadding))));
  Start->OnClickHandler = onClick;

  MainItems->Add(Start, FDIR_DOWN);

  // Load menu button
  Load = new TitleButton(
      1, MenuEntriesSprites[1], MenuEntriesHSprites[1], ItemHighlightSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) +
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (1 * ItemPadding))));
  Load->OnClickHandler = onClick;
  MainItems->Add(Load, FDIR_DOWN);

  // Extra menu button
  Extra = new TitleButton(
      2, MenuEntriesSprites[2], MenuEntriesHSprites[2], ItemHighlightSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) +
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (2 * ItemPadding))));
  Extra->OnClickHandler = onClick;
  MainItems->Add(Extra, FDIR_DOWN);

  // System menu button
  System = new TitleButton(
      3, MenuEntriesSprites[3], MenuEntriesHSprites[3], ItemHighlightSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) +
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (3 * ItemPadding))));
  System->OnClickHandler = onClick;
  MainItems->Add(System, FDIR_DOWN);

  // Quick Load secondary Load menu button
  QuickLoad = new TitleButton(0, MenuEntriesSprites[4], MenuEntriesHSprites[4],
                              SecondaryItemHighlightSprite,
                              glm::vec2(SecondaryItemX, ItemLoadQuickY));
  QuickLoad->OnClickHandler = secondaryOnClick;
  QuickLoad->IsSubButton = true;
  QuickLoad->LineDecoration = LineSprites[2];
  QuickLoad->LineY = SecondaryMenuLoadLineY;
  LoadItems->Add(QuickLoad, FDIR_DOWN);

  // Sub Load secondary Load menu button
  SubLoad = new TitleButton(1, MenuEntriesSprites[5], MenuEntriesHSprites[5],
                            SecondaryItemHighlightSprite,
                            glm::vec2(SecondaryItemX, ItemLoadY));
  SubLoad->OnClickHandler = secondaryOnClick;
  SubLoad->IsSubButton = true;
  SubLoad->LineDecoration = LineSprites[3];
  SubLoad->LineY = SecondaryMenuLoadQuickLineY;
  LoadItems->Add(SubLoad, FDIR_DOWN);

  // Clear List secondary Extra menu button
  ClearList = new TitleButton(0, MenuEntriesSprites[6], MenuEntriesHSprites[6],
                              SecondaryItemHighlightSprite,
                              glm::vec2(SecondaryItemX, ItemClearListY));
  ClearList->OnClickHandler = secondaryOnClick;
  ClearList->IsSubButton = true;
  ClearList->LineDecoration = LineSprites[0];
  ClearList->LineY = SecondaryMenuExtraClearY;
  ExtraItems->Add(ClearList, FDIR_DOWN);

  // CG Library secondary Extra menu button
  CGLibrary = new TitleButton(1, MenuEntriesSprites[7], MenuEntriesHSprites[7],
                              SecondaryItemHighlightSprite,
                              glm::vec2(SecondaryItemX, ItemCGLibraryY));
  CGLibrary->OnClickHandler = secondaryOnClick;
  CGLibrary->IsSubButton = true;
  CGLibrary->LineDecoration = LineSprites[1];
  CGLibrary->LineY = SecondaryMenuExtraCGY;
  ExtraItems->Add(CGLibrary, FDIR_DOWN);

  // Sound Library secondary Extra menu button
  SoundLibrary =
      new TitleButton(2, MenuEntriesSprites[8], MenuEntriesHSprites[8],
                      SecondaryItemHighlightSprite,
                      glm::vec2(SecondaryItemX, ItemSoundLibraryY));
  SoundLibrary->OnClickHandler = secondaryOnClick;
  SoundLibrary->IsSubButton = true;
  SoundLibrary->LineDecoration = LineSprites[2];
  SoundLibrary->LineY = SecondaryMenuExtraSoundY;
  ExtraItems->Add(SoundLibrary, FDIR_DOWN);

  // Movie Library secondary Extra menu button
  MovieLibrary =
      new TitleButton(3, MenuEntriesSprites[9], MenuEntriesHSprites[9],
                      SecondaryItemHighlightSprite,
                      glm::vec2(SecondaryItemX, ItemMovieLibraryY));
  MovieLibrary->OnClickHandler = secondaryOnClick;
  MovieLibrary->IsSubButton = true;
  MovieLibrary->LineDecoration = LineSprites[3];
  MovieLibrary->LineY = SecondaryMenuExtraMovieY;
  ExtraItems->Add(MovieLibrary, FDIR_DOWN);

  // Tips secondary Extra menu button
  Tips = new TitleButton(4, MenuEntriesSprites[10], MenuEntriesHSprites[10],
                         SecondaryItemHighlightSprite,
                         glm::vec2(SecondaryItemX, ItemTipsY));
  Tips->OnClickHandler = secondaryOnClick;
  Tips->IsSubButton = true;
  Tips->LineDecoration = LineSprites[4];
  Tips->LineY = SecondaryMenuExtraTipsY;
  ExtraItems->Add(Tips, FDIR_DOWN);

  // Trophy secondary Extra menu button
  Trophy = new TitleButton(5, MenuEntriesSprites[11], MenuEntriesHSprites[11],
                           SecondaryItemHighlightSprite,
                           glm::vec2(SecondaryItemX, ItemTrophyY));
  Trophy->OnClickHandler = secondaryOnClick;
  Trophy->IsSubButton = true;
  Trophy->LineDecoration = LineSprites[5];
  Trophy->LineY = SecondaryMenuExtraTrophyY;
  ExtraItems->Add(Trophy, FDIR_DOWN);

  // Option secondary System menu button
  Config = new TitleButton(0, MenuEntriesSprites[12], MenuEntriesHSprites[12],
                           SecondaryItemHighlightSprite,
                           glm::vec2(SecondaryItemX, ItemConfigY));
  Config->OnClickHandler = secondaryOnClick;
  Config->IsSubButton = true;
  Config->LineDecoration = LineSprites[2];
  Config->LineY = SecondaryMenuSystemConfigY;
  SystemItems->Add(Config, FDIR_DOWN);

  // System Save secondary System menu button
  SystemSave = new TitleButton(
      1, MenuEntriesSprites[13], MenuEntriesHSprites[13],
      SecondaryItemHighlightSprite, glm::vec2(SecondaryItemX, ItemSystemSaveY));
  SystemSave->OnClickHandler = secondaryOnClick;
  SystemSave->IsSubButton = true;
  SystemSave->LineDecoration = LineSprites[3];
  SystemSave->LineY = SecondaryMenuSystemSaveY;
  SystemItems->Add(SystemSave, FDIR_DOWN);
}

void TitleMenu::Show() {
  if (State != Shown) {
    State = Shown;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    if (PressToStartAnimation.State == AS_Stopped) {
      PressToStartAnimation.StartIn();
      SpinningCircleAnimation.StartIn();
    }
  }
}
void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hidden;
    MainItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput();

  PressToStartAnimation.Update(dt);
  SpinningCircleAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    ItemsFadeInAnimation.StartOut();
    SecondaryItemsFadeInAnimation.StartOut();
    Hide();
  }

  if (State == Shown && IsFocused) {
    MainItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
    MainItems->Update(dt);
    LoadItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    LoadItems->Update(dt);
    ExtraItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    ExtraItems->Update(dt);
    SystemItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    SystemItems->Update(dt);

    switch (ScrWork[SW_TITLEDISPCT]) {
      case 0: {
        // When returning to title menu from loading a game we need to hide the
        // load sub-menu
        if (LoadItems->IsShown) {
          SecondaryFadeAnimation.StartOut();
          MainItems->HasFocus = true;
          LoadItems->Hide();
        }
      } break;
      case 3: {  // Main Menu Fade In
        if (!MainItems->IsShown && ScrWork[SW_TITLECT] == 0) {
          MainItems->Show();
          MainItems->Tint.a = 0.0f;
          CurrentlyFocusedElement = Start;
          Start->HasFocus = true;
          PrimaryFadeAnimation.DurationIn = PrimaryFadeInDuration;
          PrimaryFadeAnimation.DurationOut = PrimaryFadeOutDuration;
          PrimaryFadeAnimation.StartIn();
        }
      } break;
      case 7: {  // Secondary menu Load Fade In
        if (!LoadItems->IsShown && ScrWork[SW_TITLECT] == 0) {
          LoadItems->Show();
          LoadItems->Tint.a = 0.0f;
          MainItems->HasFocus = false;
          CurrentlyFocusedElement = QuickLoad;
          QuickLoad->HasFocus = true;

          SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
          SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;
          SecondaryFadeAnimation.StartIn();

        } else if (LoadItems->IsShown && ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

        } else if (ScrWork[SW_TITLECT] == 0) {
          LoadItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case 9: {  // Secondary menu Extra Fade In
        if (!ExtraItems->IsShown && ScrWork[SW_TITLECT] == 0) {
          ExtraItems->Show();
          ExtraItems->Tint.a = 0.0f;
          MainItems->HasFocus = false;
          CurrentlyFocusedElement = ClearList;
          ClearList->HasFocus = true;

          SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
          SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;
          SecondaryFadeAnimation.StartIn();

        } else if (ExtraItems->IsShown && ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

        } else if (ScrWork[SW_TITLECT] == 0) {
          ExtraItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case 11: {  // Secondary menu System Fade In
        if (!SystemItems->IsShown && ScrWork[SW_TITLECT] == 0) {
          SystemItems->Show();
          SystemItems->Tint.a = 0.0f;
          MainItems->HasFocus = false;
          CurrentlyFocusedElement = Config;
          Config->HasFocus = true;

          SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
          SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;
          SecondaryFadeAnimation.StartIn();

        } else if (SystemItems->IsShown && ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

        } else if (ScrWork[SW_TITLECT] == 0) {
          SystemItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
    }
    if (PressToStartAnimation.State == AS_Stopped &&
        ScrWork[SW_TITLEDISPCT] == 1) {
      PressToStartAnimation.StartIn();
      SpinningCircleAnimation.StartIn();
    }
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    if (ScrWork[SW_MENUCT] < 64) {
      switch (ScrWork[SW_TITLEDISPCT]) {
        case 0:  // Initial animation
          break;
        case 1: {  // Press to start
          DrawTitleMenuBackGraphics();
          glm::vec4 col = glm::vec4(1.0f);
          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
          Renderer2D::DrawSprite(PressToStartSprite,
                                 glm::vec2(PressToStartX, PressToStartY), col);
        } break;
        case 2: {  // Transition between Press to start and menus
          DrawTitleMenuBackGraphics();
        } break;
        case 3:  // MenuItems Fade In
          if (ItemsFadeInAnimation.IsOut() &&
              ItemsFadeInAnimation.State != AS_Playing)
            ItemsFadeInAnimation.StartIn();
          else if (ItemsFadeInAnimation.State != AS_Playing)
            ItemsFadeInAnimation.StartOut();
          DrawTitleMenuBackGraphics();
          MainItems->Render();
        case 4: {  // Main Menu
          DrawTitleMenuBackGraphics();
          MainItems->Render();
        } break;
        case 7: {  // Secondary menu LOAD Fade In
          if (SecondaryItemsFadeInAnimation.IsOut() &&
              SecondaryItemsFadeInAnimation.State != AS_Playing)
            SecondaryItemsFadeInAnimation.StartIn();
          else if (SecondaryItemsFadeInAnimation.State != AS_Playing)
            SecondaryItemsFadeInAnimation.StartOut();
        }
        case 8: {  // Secondary menu LOAD
          DrawTitleMenuBackGraphics();
          LoadItems->Render();
          MainItems->Render();
        } break;
        case 9: {  // Secondary menu EXTRAS Fade In
          DrawTitleMenuBackGraphics();
          MainItems->Render();
        } break;
        case 10: {  // Secondary menu EXTRAS
          DrawTitleMenuBackGraphics();
          ExtraItems->Render();
          MainItems->Render();
        } break;
        case 11: {  // Secondary menu SYSTEM Fade In
          DrawTitleMenuBackGraphics();
          MainItems->Render();
        }
        case 12: {  // Secondary menu SYSTEM
          DrawTitleMenuBackGraphics();
          SystemItems->Render();
          MainItems->Render();
        } break;
      }
    }

    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
    Renderer2D::DrawRect(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), col);
  }
}

inline void TitleMenu::DrawTitleMenuBackGraphics() {
  Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
  Renderer2D::DrawSprite(SpinningCircleSprite,
                         glm::vec2(SpinningCircleX, SpinningCircleY),
                         glm::vec4(1.0f), glm::vec2(2.0f),
                         -SpinningCircleAnimation.Progress * 2.0f * M_PI);
  Renderer2D::DrawSprite(DelusionADVUnderSprite,
                         glm::vec2(DelusionADVUnderX, DelusionADVUnderY));
  Renderer2D::DrawSprite(DelusionADVSprite,
                         glm::vec2(DelusionADVX, DelusionADVY));
  Renderer2D::DrawSprite(SeiraUnderSprite, glm::vec2(SeiraUnderX, SeiraUnderY));
  Renderer2D::DrawSprite(SeiraSprite, glm::vec2(SeiraX, SeiraY));
  Renderer2D::DrawSprite(CHLogoSprite, glm::vec2(CHLogoX, CHLogoY));
  Renderer2D::DrawSprite(LCCLogoUnderSprite,
                         glm::vec2(LCCLogoUnderX, LCCLogoUnderY));
  Renderer2D::DrawSprite(ChuLeftLogoSprite,
                         glm::vec2(ChuLeftLogoX, ChuLeftLogoY));
  Renderer2D::DrawSprite(ChuRightLogoSprite,
                         glm::vec2(ChuRightLogoX, ChuRightLogoY));
  Renderer2D::DrawSprite(LoveLogoSprite, glm::vec2(LoveLogoX, LoveLogoY));
  Renderer2D::DrawSprite(StarLogoSprite, glm::vec2(StarLogoX, StarLogoY));
  Renderer2D::DrawSprite(ExclMarkLogoSprite,
                         glm::vec2(ExclMarkLogoX, ExclMarkLogoY));
  Renderer2D::DrawSprite(CopyrightTextSprite,
                         glm::vec2(CopyrightTextX, CopyrightTextY));
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto