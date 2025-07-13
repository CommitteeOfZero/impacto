#include "clearlistmenu.h"

#include "../../profile/games/cclcc/clearlistmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::ClearListMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

ClearListMenu::ClearListMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void ClearListMenu::Show() {
  if (State != Showing) {
    State = Showing;

    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;

    FadeAnimation.StartIn();
  }
}

void ClearListMenu::Hide() {
  if (State != Hiding) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
    } else {
      UI::FocusedMenu = 0;
    }
  }
}

void ClearListMenu::Update(float dt) {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 7)) {
    Show();
  }

  if (State != Hidden) {
    FadeAnimation.Update(dt);
  }

  if (State == Shown && ScrWork[SW_SYSSUBMENUNO] == 7) {
    UpdateInput();
  }

  if (State == Showing && FadeAnimation.Progress == 1.0f &&
      ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }
}

void ClearListMenu::Render() {
  if (State != Hidden) {
    glm::vec4 transition(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    glm::vec4 maskTint = glm::vec4(1.0f);
    maskTint.a = 0.85f;
    Renderer->DrawSprite(ClearListBookLayerSprite, glm::vec2(0.0f, MenuOffsetY),
                         transition);
    DrawEndingSprites(transition);
    Renderer->DrawSprite(
        ClearListMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);
    Renderer->DrawSprite(ClearListGuideSprite, ClearListGuidePosition,
                         transition);
  }
}

int mappedAlphas[] = {
    4,  // 801
    1,  // 802
    6,  // 803
    2,  // 804
    5,  // 805
    3,  // 806
    7,  // 807
    8,  // 808
    0,  // 809
    9   // 810
};

void ClearListMenu::DrawEndingSprites(const glm::vec4& transition) {
  int alphas[10]{};
  int tmp = ScrWork[SW_CLRALPHA] << 3;
  for (int i = 0; i < Endings; i++) {
    alphas[i] = std::clamp(tmp, 0, 256) * ScrWork[SW_SYSSUBMENUALPHA] >> 8;
    tmp -= 32;
  }

  for (int i = 0; i < Endings; i++) {
    if (GetFlag(SF_CLR_END1 + i)) {
      Renderer->DrawSprite(
          EndingSprites[i],
          glm::vec2(
              EndingSprites[i].Bounds.X,
              EndingSprites[i].Bounds.Y - EndingSpriteOffsetY + MenuOffsetY),
          glm::vec4{
              glm::vec3{transition},
              transition.a *
                  (State != Hiding ? alphas[mappedAlphas[i]] / 256.0f : 1.0f)});
    }
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto