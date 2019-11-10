#include "titlemenu.h"

#include "../../profile/hud/titlemenu.h"
#include "../../profile/games/chlcc/titlemenu.h"
#include "../../renderer2d.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../io/vfs.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CHLCC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

int SecondaryChoiceCount = 0;

void TitleMenu::Show() {
  if (State != Shown) {
    State = Showing;
  }
}
void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
  }
}
void TitleMenu::Update(float dt) {
  PressToStartAnimation.Update(dt);
  SpinningCircleAnimation.Update(dt);
  ItemsFadeInAnimation.Update(dt);
  SecondaryItemsFadeInAnimation.Update(dt);

  if (Input::KeyboardButtonWentDown[SDL_SCANCODE_DOWN]) {
    if (ScrWork[SW_TITLEDISPCT] >= 7) {
      SecondaryChoice++;
      if (SecondaryChoice > SecondaryChoiceCount) SecondaryChoice = 0;
    } else {
      CurrentChoice++;
      if (CurrentChoice > 3) CurrentChoice = 0;
    }
  } else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_UP]) {
    if (ScrWork[SW_TITLEDISPCT] >= 7) {
      SecondaryChoice--;
      if (SecondaryChoice < 0) SecondaryChoice = SecondaryChoiceCount;
    } else {
      CurrentChoice--;
      if (CurrentChoice < 0) CurrentChoice = 3;
    }
  }

  if (ScrWork[SW_TITLEDISPCT] == 1) {
    Show();
  } else if (ScrWork[SW_TITLEDISPCT] == 0) {
    Hide();
  }

  if (PressToStartAnimation.State == AS_Stopped &&
      ScrWork[SW_TITLEDISPCT] == 1) {
    PressToStartAnimation.StartIn();
    SpinningCircleAnimation.StartIn();
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
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
      case 3: {  // MenuItems Fade In
        if (ItemsFadeInAnimation.IsOut() &&
            ItemsFadeInAnimation.State != AS_Playing)
          ItemsFadeInAnimation.StartIn();
        else if (ItemsFadeInAnimation.State != AS_Playing)
          ItemsFadeInAnimation.StartOut();
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItems();
      } break;
      case 4: {  // Main Menu
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItems();
      } break;
      case 7: {  // Secondary menu LOAD Fade In
        SecondaryChoiceCount = 1;
        if (SecondaryItemsFadeInAnimation.IsOut() &&
            SecondaryItemsFadeInAnimation.State != AS_Playing)
          SecondaryItemsFadeInAnimation.StartIn();
        else if (SecondaryItemsFadeInAnimation.State != AS_Playing)
          SecondaryItemsFadeInAnimation.StartOut();
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItemsLoad();
        DrawTitleMenuItems();
      } break;
      case 8: {  // Secondary menu LOAD
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItemsLoad();
        DrawTitleMenuItems();
      } break;
      case 9: {  // Secondary menu EXTRAS Fade In
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItems();
      } break;
      case 10: {  // Secondary menu EXTRAS
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItems();
      } break;
      case 11: {  // Secondary menu SYSTEM Fade In
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItems();
      } break;
      case 12: {  // Secondary menu SYSTEM
        DrawTitleMenuBackGraphics();
        DrawTitleMenuItems();
      } break;
    }
  }
}

void TitleMenu::DrawTitleMenuBackGraphics() {
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

void TitleMenu::DrawTitleMenuItems() {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, ItemsFadeInAnimation.Progress);
  for (int i = 0; i < 4; i++) {
    if (CurrentChoice == i) {
      Renderer2D::DrawSprite(
          ItemHighlightSprite,
          glm::vec2(
              ((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress) - 1.0f) -
                  ItemHighlightOffsetX,
              ((ItemYBase - 1.0f) + (i * ItemPadding)) - ItemHighlightOffsetY),
          col);
      Renderer2D::DrawSprite(
          MenuEntriesHSprites[i],
          glm::vec2(ItemHighlightOffsetX * ItemsFadeInAnimation.Progress,
                    ItemYBase + (i * ItemPadding)),
          col);
    } else {
      Renderer2D::DrawSprite(
          MenuEntriesSprites[i],
          glm::vec2(ItemHighlightOffsetX * ItemsFadeInAnimation.Progress,
                    ItemYBase + (i * ItemPadding)),
          col);
    }
  }
}

void TitleMenu::DrawTitleMenuItemsLoad() {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, SecondaryItemsFadeInAnimation.Progress);

  Renderer2D::DrawSprite(SecondaryItemHighlightSprite,
                         glm::vec2(SecondaryItemHighlightX,
                                   SecondaryChoice * SecondaryMenuPaddingY +
                                       SecondaryMenuLoadOffsetY),
                         col);
  if (SecondaryChoice == 0) {
    Renderer2D::DrawSprite(
        ItemLoadQuickLineSprite,
        glm::vec2(SecondaryMenuLoadLineX, SecondaryMenuLoadQuickLineY), col);
    Renderer2D::DrawSprite(ItemLoadQuickHighlightedSprite,
                           glm::vec2(ItemLoadX, ItemLoadQuickY), col);
    Renderer2D::DrawSprite(ItemLoadSprite, glm::vec2(ItemLoadX, ItemLoadY),
                           col);
  } else {
    Renderer2D::DrawSprite(
        ItemLoadLineSprite,
        glm::vec2(SecondaryMenuLoadLineX, SecondaryMenuLoadLineY), col);
    Renderer2D::DrawSprite(ItemLoadQuickSprite,
                           glm::vec2(ItemLoadX, ItemLoadQuickY), col);
    Renderer2D::DrawSprite(ItemLoadHighlightedSprite,
                           glm::vec2(ItemLoadX, ItemLoadY), col);
  }
}
void TitleMenu::DrawTitleMenuItemsExtras() {}
void TitleMenu::DrawTitleMenuItemsSystem() {}

}  // namespace CHLCC
}  // namespace Impacto