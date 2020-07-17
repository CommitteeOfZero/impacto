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
  MainItems = new WidgetGroup();
  LoadItems = new WidgetGroup();
  SystemItems = new WidgetGroup();

  auto onClick =
      std::bind(&TitleMenu::MenuButtonOnClick, this, std::placeholders::_1);
  auto secondaryOnClick = std::bind(&TitleMenu::SecondaryButtonOnClick, this,
                                    std::placeholders::_1);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  // Start menu button
  Start = new TitleButton(
      0, MenuEntriesSprites[0], MenuEntriesHSprites[0], nullSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) -
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (1 * ItemPadding)) - ItemHighlightOffsetY));
  Start->OnClickHandler = onClick;
  MainItems->Add(Start, FocusDirection::Vertical);

  // Load menu button
  Load = new TitleButton(
      1, MenuEntriesSprites[1], MenuEntriesHSprites[1], nullSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) -
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (2 * ItemPadding)) - ItemHighlightOffsetY));
  Load->OnClickHandler = onClick;
  MainItems->Add(Load, FocusDirection::Vertical);
  // Extra menu button
  Extra = new TitleButton(
      2, MenuEntriesSprites[2], MenuEntriesHSprites[2], nullSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) -
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (2 * ItemPadding)) - ItemHighlightOffsetY));
  Extra->OnClickHandler = onClick;
  MainItems->Add(Extra, FocusDirection::Vertical);

  // System menu button
  System = new TitleButton(
      3, MenuEntriesSprites[3], MenuEntriesHSprites[3], nullSprite,
      glm::vec2(
          ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) -
              ItemHighlightOffsetX,
          ((ItemYBase - 1.0f) + (3 * ItemPadding)) - ItemHighlightOffsetY));
  System->OnClickHandler = onClick;
  MainItems->Add(System, FocusDirection::Vertical);
  /*
    // Sub Load secondary Load menu button
    SubLoad = new TitleButton(
        0, MenuEntriesSprites[4], MenuEntriesHSprites[4], nullSprite,
        glm::vec2(MenuEntriesX, MenuEntriesFirstY + (3 * MenuEntriesYPadding)));
    SubLoad->OnClickHandler = secondaryOnClick;
    LoadItems->Add(SubLoad, FocusDirection::Vertical);

    // Quick Load secondary Continue menu button
    QuickLoad = new TitleButton(
        1, MenuEntriesSprites[5], MenuEntriesHSprites[5], nullSprite,
        glm::vec2(MenuEntriesX - 20.0f,
                  MenuEntriesFirstY + (4 * MenuEntriesYPadding)));
    QuickLoad->OnClickHandler = secondaryOnClick;
    LoadItems->Add(QuickLoad, FocusDirection::Vertical);

    // Option secondary System menu button
    Config = new TitleButton(
        0, MenuEntriesSprites[6], MenuEntriesHSprites[6], nullSprite,
        glm::vec2(MenuEntriesX, MenuEntriesFirstY + (6 * MenuEntriesYPadding)));
    Config->OnClickHandler = secondaryOnClick;
    SystemItems->Add(Config, FocusDirection::Vertical);

    // System Save secondary System menu button
    SystemSave = new TitleButton(
        1, MenuEntriesSprites[7], MenuEntriesHSprites[7], nullSprite,
        glm::vec2(MenuEntriesX - 20.0f,
                  MenuEntriesFirstY + (7 * MenuEntriesYPadding)));
    SystemSave->OnClickHandler = secondaryOnClick;
    SystemItems->Add(SystemSave, FocusDirection::Vertical);*/
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
    if (PressToStartAnimation.State == AS_Stopped)
      PressToStartAnimation.StartIn();
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
  PressToStartAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State == Shown && IsFocused) {
    MainItems->Opacity =
        glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
    MainItems->Update(dt);
    LoadItems->Opacity =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    LoadItems->Update(dt);
    SystemItems->Opacity =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    SystemItems->Update(dt);
    /*
    switch (ScrWork[SW_TITLEDISPCT]) {
      case 0: {
        // When returning to title menu from loading a game we need to hide the
        // load sub-menu
        if (LoadItems->IsShown) {
          SecondaryFadeAnimation.StartOut();
          MainItems->HasFocus = true;
          Load->Move(glm::vec2(-110.0f, 0.0f));
          QuickLoad->Move(glm::vec2(-130.0f, 0.0f));
          System->Move(glm::vec2(0.0f, -2.0f * MenuEntriesYPadding));
          LoadItems->Hide();
        }
      } break;
      case 3: {  // Main Menu Fade In
        if (!MainItems->IsShown && ScrWork[SW_TITLECT] == 0) {
          MainItems->Show();
          MainItems->Opacity = 0.0f;
          PrimaryFadeAnimation.DurationIn = 0.3f;
          PrimaryFadeAnimation.DurationOut = 0.3f;
          PrimaryFadeAnimation.StartIn();
        }
      } break;
      case 7: {  // Secondary menu Load Fade In
        if (!LoadItems->IsShown && ScrWork[SW_TITLECT] == 0) {
          LoadItems->Show();
          LoadItems->Opacity = 0.0f;
          MainItems->HasFocus = false;

          SecondaryFadeAnimation.DurationIn = 0.512f;
          SecondaryFadeAnimation.DurationOut = 0.512f;
          SecondaryFadeAnimation.StartIn();

          Load->Move(glm::vec2(110.0f, 0.0f), 0.512f);
          QuickLoad->Move(glm::vec2(130.0f, 0.0f), 0.512f);

          System->Move(glm::vec2(0.0f, 2.0f * MenuEntriesYPadding), 0.512f);
        } else if (LoadItems->IsShown && ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

          Load->Move(glm::vec2(-110.0f, 0.0f), 0.512f);
          QuickLoad->Move(glm::vec2(-130.0f, 0.0f), 0.512f);

          System->Move(glm::vec2(0.0f, -2.0f * MenuEntriesYPadding), 0.512f);
        } else if (ScrWork[SW_TITLECT] == 0) {
          LoadItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case 11: {
        if (!SystemItems->IsShown && ScrWork[SW_TITLECT] == 0) {
          SystemItems->Show();
          SystemItems->Opacity = 0.0f;
          MainItems->HasFocus = false;

          SecondaryFadeAnimation.DurationIn = 0.512f;
          SecondaryFadeAnimation.DurationOut = 0.512f;
          SecondaryFadeAnimation.StartIn();

          Config->Move(glm::vec2(110.0f, 0.0f), 0.512f);
          SystemSave->Move(glm::vec2(130.0f, 0.0f), 0.512f);

        } else if (SystemItems->IsShown && ScrWork[SW_TITLECT] == 32) {
          SecondaryFadeAnimation.StartOut();

          Config->Move(glm::vec2(-110.0f, 0.0f), 0.512f);
          SystemSave->Move(glm::vec2(-130.0f, 0.0f), 0.512f);

        } else if (ScrWork[SW_TITLECT] == 0) {
          SystemItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
    }*/
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    if (ScrWork[SW_MENUCT] != 100) {
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
          glm::vec4 col = glm::vec4(1.0f);
          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
          Renderer2D::DrawSprite(PressToStartSprite,
                                 glm::vec2(PressToStartX, PressToStartY), col);
        } break;
        case 3:    // Main Menu Fade In
        case 4: {  // Main Menu
          Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
          MainItems->Render();
        } break;
        case 7:    // Secondary menu Continue Fade In
        case 8: {  // Secondary menu Continue
          Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
          LoadItems->Render();
          MainItems->Render();
        } break;
        case 11:    // Secondary menu System Fade In
        case 12: {  // Secondary menu System
          Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
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

//
// int SecondaryChoiceCount = 0;
//
// void TitleMenu::Show() {
//  if (State != Shown) {
//    State = Shown;
//  }
//}
// void TitleMenu::Hide() {
//  if (State != Hidden) {
//    State = Hidden;
//  }
//}
// void TitleMenu::Update(float dt) {
//  PressToStartAnimation.Update(dt);
//  SpinningCircleAnimation.Update(dt);
//  ItemsFadeInAnimation.Update(dt);
//  SecondaryItemsFadeInAnimation.Update(dt);
//
//  if (GetFlag(SF_TITLEMODE)) {
//    Show();
//  } else {
//    ItemsFadeInAnimation.StartOut();
//    SecondaryItemsFadeInAnimation.StartOut();
//    Hide();
//  }
//
//  if (Input::KeyboardButtonWentDown[SDL_SCANCODE_DOWN]) {
//    if (ScrWork[SW_TITLEDISPCT] >= 7) {
//      SecondaryChoice++;
//      if (SecondaryChoice > SecondaryChoiceCount) SecondaryChoice = 0;
//    } else {
//      CurrentChoice++;
//      if (CurrentChoice > 3) CurrentChoice = 0;
//    }
//  } else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_UP]) {
//    if (ScrWork[SW_TITLEDISPCT] >= 7) {
//      SecondaryChoice--;
//      if (SecondaryChoice < 0) SecondaryChoice = SecondaryChoiceCount;
//    } else {
//      CurrentChoice--;
//      if (CurrentChoice < 0) CurrentChoice = 3;
//    }
//  }
//
//  if (PressToStartAnimation.State == AS_Stopped &&
//      ScrWork[SW_TITLEDISPCT] == 1) {
//    PressToStartAnimation.StartIn();
//    SpinningCircleAnimation.StartIn();
//  }
//}
//
// void TitleMenu::Render() {
//  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
//    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
//    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
//    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
//    Renderer2D::DrawRect(
//        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), col);
//    if (ScrWork[SW_SYSMENUCT] < 64) {
//      switch (ScrWork[SW_TITLEDISPCT]) {
//        case 0:  // Initial animation
//          break;
//        case 1: {  // Press to start
//          DrawTitleMenuBackGraphics();
//          glm::vec4 col = glm::vec4(1.0f);
//          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
//          Renderer2D::DrawSprite(PressToStartSprite,
//                                 glm::vec2(PressToStartX, PressToStartY),
//                                 col);
//        } break;
//        case 2: {  // Transition between Press to start and menus
//          DrawTitleMenuBackGraphics();
//        } break;
//        case 3: {  // MenuItems Fade In
//          if (ItemsFadeInAnimation.IsOut() &&
//              ItemsFadeInAnimation.State != AS_Playing)
//            ItemsFadeInAnimation.StartIn();
//          else if (ItemsFadeInAnimation.State != AS_Playing)
//            ItemsFadeInAnimation.StartOut();
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItems();
//        } break;
//        case 4: {  // Main Menu
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItems();
//        } break;
//        case 7: {  // Secondary menu LOAD Fade In
//          SecondaryChoiceCount = 1;
//          if (SecondaryItemsFadeInAnimation.IsOut() &&
//              SecondaryItemsFadeInAnimation.State != AS_Playing)
//            SecondaryItemsFadeInAnimation.StartIn();
//          else if (SecondaryItemsFadeInAnimation.State != AS_Playing)
//            SecondaryItemsFadeInAnimation.StartOut();
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItemsLoad();
//          DrawTitleMenuItems();
//        } break;
//        case 8: {  // Secondary menu LOAD
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItemsLoad();
//          DrawTitleMenuItems();
//        } break;
//        case 9: {  // Secondary menu EXTRAS Fade In
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItems();
//        } break;
//        case 10: {  // Secondary menu EXTRAS
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItems();
//        } break;
//        case 11: {  // Secondary menu SYSTEM Fade In
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItems();
//        } break;
//        case 12: {  // Secondary menu SYSTEM
//          DrawTitleMenuBackGraphics();
//          DrawTitleMenuItems();
//        } break;
//      }
//    }
//  }
//}
//
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

// void titlemenu::drawtitlemenuitems() {
//  glm::vec4 col = glm::vec4(1.0f);
//  col.a = glm::smoothstep(0.0f, 1.0f, itemsfadeinanimation.progress);
//  for (int i = 0; i < 4; i++) {
//    if (currentchoice == i) {
//      renderer2d::drawsprite(
//          itemhighlightsprite,
//          glm::vec2(
//              ((itemhighlightoffsetx * itemsfadeinanimation.progress) - 1.0f)
//              -
//                  itemhighlightoffsetx,
//              ((itemybase - 1.0f) + (i * itempadding)) -
//              itemhighlightoffsety),
//          col);
//      renderer2d::drawsprite(
//          menuentrieshsprites[i],
//          glm::vec2(itemhighlightoffsetx * itemsfadeinanimation.progress,
//                    itemybase + (i * itempadding)),
//          col);
//    } else {
//      renderer2d::drawsprite(
//          menuentriessprites[i],
//          glm::vec2(itemhighlightoffsetx * itemsfadeinanimation.progress,
//                    itemybase + (i * itempadding)),
//          col);
//    }
//  }
//}
//
// void titlemenu::drawtitlemenuitemsload() {
//  glm::vec4 col = glm::vec4(1.0f);
//  col.a = glm::smoothstep(0.0f, 1.0f, secondaryitemsfadeinanimation.progress);
//
//  renderer2d::drawsprite(secondaryitemhighlightsprite,
//                         glm::vec2(secondaryitemhighlightx,
//                                   secondarychoice * secondarymenupaddingy +
//                                       secondarymenuloadoffsety),
//                         col);
//  if (secondarychoice == 0) {
//    renderer2d::drawsprite(
//        itemloadquicklinesprite,
//        glm::vec2(secondarymenuloadlinex, secondarymenuloadquickliney), col);
//    renderer2d::drawsprite(itemloadquickhighlightedsprite,
//                           glm::vec2(itemloadx, itemloadquicky), col);
//    renderer2d::drawsprite(itemloadsprite, glm::vec2(itemloadx, itemloady),
//                           col);
//  } else {
//    renderer2d::drawsprite(
//        itemloadlinesprite,
//        glm::vec2(secondarymenuloadlinex, secondarymenuloadliney), col);
//    renderer2d::drawsprite(itemloadquicksprite,
//                           glm::vec2(itemloadx, itemloadquicky), col);
//    renderer2d::drawsprite(itemloadhighlightedsprite,
//                           glm::vec2(itemloadx, itemloady), col);
//  }
//}
// void TitleMenu::DrawTitleMenuItemsExtras() {}
// void TitleMenu::DrawTitleMenuItemsSystem() {}
//
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto