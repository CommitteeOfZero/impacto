#include "systemmenu.h"

#include "../../profile/ui/systemmenu.h"
#include "../../profile/games/rne/systemmenu.h"
#include "../../games/rne/tilebackground.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/rne/sysmenubutton.h"

namespace Impacto {
namespace UI {
namespace RNE {

using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::Profile::RNE::SystemMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::RNE;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  ScrWork[SW_SYSMENUCNO] = target->Id;
  ChoiceMade = true;
}

SystemMenu::SystemMenu() {
  MainItems = new Widgets::Group(this);

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  for (int i = 0; i < MenuEntriesNum; i++) {
    SysMenuButton* menuButton = new SysMenuButton(
        i, MenuEntriesSprites[i], nullSprite, MenuEntriesHSprites[i],
        glm::vec2(*MenuEntriesX,
                  *MenuEntriesFirstY + (*MenuEntriesYPadding * i)));

    menuButton->OnClickHandler = onClick;
    MainItems->Add(menuButton, FDIR_DOWN);
  }
}

void SystemMenu::Show() {
  if (State != Shown) {
    State = Showing;
    if (BackgroundAnimation) BackgroundAnimation->StartIn();

    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void SystemMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    HighlightAnimation.StartOut();
    for (auto& item : MainItems->Children) {
      item->HasFocus = false;
    }
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SystemMenu::Update(float dt) {
  UpdateInput(dt);

  if (ScrWork[SW_SYSMENUALPHA] < 256 && State == Shown) {
    Hide();
  }

  if (ScrWork[SW_SYSMENUCT] == 100 && State == Shown) {
    Hide();
  }

  SkyMoveAnimation.Update(dt);
  EntriesMoveAnimation.Update(dt);
  HighlightAnimation.Update(dt);
  if (BackgroundAnimation) BackgroundAnimation->Update(dt);

  if (State == Showing) {
    if (BackgroundAnimation->Progress >= SkyInStartProgress) {
      if (SkyMoveAnimation.IsOut()) SkyMoveAnimation.StartIn();
      if (EntriesMoveAnimation.IsOut()) EntriesMoveAnimation.StartIn();
    }
    if (SkyMoveAnimation.IsIn()) {
      if (HighlightAnimation.IsOut()) HighlightAnimation.StartIn();
    }
    if (HighlightAnimation.IsIn()) State = Shown;
  } else if (State == Hiding) {
    if (HighlightAnimation.IsOut()) {
      if (BackgroundAnimation && BackgroundAnimation->IsIn())
        BackgroundAnimation->StartOut();
    }
    if (BackgroundAnimation->Progress <= SkyOutStartProgress) {
      if (SkyMoveAnimation.IsIn()) SkyMoveAnimation.StartOut();
      if (EntriesMoveAnimation.IsIn()) EntriesMoveAnimation.StartOut();
    }
    if (BackgroundAnimation->IsOut()) State = Hidden;
  }
  ButtonBackgroundSprite.Bounds.X =
      ButtonBackgroundSprStartX -
      (ButtonBackgroundTargetWidth * SkyMoveAnimation.Progress);
  ButtonBackgroundSprite.Bounds.Width =
      ButtonBackgroundTargetWidth * SkyMoveAnimation.Progress;

  int idx = 0;
  for (auto& item : MainItems->Children) {
    item->MoveTo(glm::vec2(((idx * MenuEntriesXSkew) + *MenuEntriesXOffset) *
                               (1.0f - EntriesMoveAnimation.Progress),
                           *MenuEntriesFirstY + (*MenuEntriesYPadding * idx)));
    item->Tint.a = glm::smoothstep(
        0.0f, 1.0f, 1.0f - (idx + 1) * (1.0f - EntriesMoveAnimation.Progress));
    Button* button = (Button*)item;
    button->HighlightSprite.Bounds.Width =
        MenuEntriesTargetWidth * HighlightAnimation.Progress;
    idx++;
  }

  if (ScrWork[SW_SYSMENUALPHA] == 0) {
    MainItems->Hide();
  }

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}

void SystemMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSMENUALPHA] > 0) {
    if (BackgroundAnimation) BackgroundAnimation->Render();
    glm::vec4 colSky(1.0f);
    colSky.a = glm::smoothstep(0.0f, 1.0f, SkyMoveAnimation.Progress);

    glm::vec2 destSky = glm::vec2(
        SkyBackgroundBeginX +
            (SkyMoveAnimation.Progress * std::abs(SkyBackgroundBeginX)),
        SkyBackgroundY);
    Renderer->DrawSprite(SkyBackgroundSprite, destSky, colSky);
    Renderer->DrawSprite(SkyArrowSprite, destSky, colSky);

    Renderer->DrawSprite(
        SkyTextSprite,
        glm::vec2(SkyTextBeginX + (SkyMoveAnimation.Progress *
                                   std::abs(SkyBackgroundBeginX)),
                  SkyTextY),
        colSky);

    Renderer->DrawSprite(
        ButtonBackgroundSprite,
        glm::vec2(ButtonBackgroundStartX -
                      (ButtonBackgroundStartX * SkyMoveAnimation.Progress),
                  ButtonBackgroundY),
        glm::vec4(1.0f));

    if (SkyMoveAnimation.IsIn()) {
      Renderer->DrawSprite(ButtonPromptsSprite,
                           glm::vec2(ButtonBackgroundX, ButtonBackgroundY),
                           glm::vec4(1.0f));
    }
    MainItems->Render();
  }
}

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto
