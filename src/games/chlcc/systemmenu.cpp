#include "systemmenu.h"
#include "../../profile/games/chlcc/systemmenu.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../vm/interface/input.h"
#include "../../profile/ui/systemmenu.h"
#include "../../ui/widgets/chlcc/systemmenuentrybutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::SystemMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::SystemMenu;
using namespace Impacto::UI::Widgets::CHLCC;

void SystemMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_SYSMENUCNO] = target->Id;
  // Make the Id match the save menu mode (5th button would be Quick Load which
  // is case 0)
  ScrWork[SW_SAVEMENUMODE] = target->Id % 4;
  ChoiceMade = true;
}

SystemMenu::SystemMenu() {
  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.DurationIn = FadeInDuration;
  MenuTransition.DurationOut = FadeOutDuration;

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  MainItems = new Widgets::Group(this);

  for (int i = 0; i < MenuEntriesNum; i++) {
    SystemMenuEntryButton* menuButton = new SystemMenuEntryButton(
        i, MenuEntriesSprites[i], MenuEntriesSprites[i],
        RgbIntToFloat(FocusTint), Sprite(), MenuEntriesPositions[i]);

    menuButton->OnClickHandler = onClick;
    MainItems->Add(menuButton, FDIR_DOWN);
  }
  MainItems->Children[0]->SetFocus(MainItems->Children[MenuEntriesNum - 1],
                                   FDIR_UP);
  MainItems->Children[MenuEntriesNum - 1]->SetFocus(MainItems->Children[0],
                                                    FDIR_DOWN);
}

void SystemMenu::Show() {
  if (State != Shown) {
    State = Showing;
    MenuTransition.StartIn();
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
    MenuTransition.StartOut();
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
  UpdateInput();
  MainItems->UpdateInput();
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_SYSTEMMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && State == Hiding) {
    MainItems->Hide();
    State = Hidden;
  } else if (MenuTransition.IsIn() && State == Showing) {
    State = Shown;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == +AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == +AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
  }

  if (State == Shown && IsFocused) {
    MainItems->Update(dt);
  }
}
void SystemMenu::Render() {
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawRect(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();
    glm::vec3 tint = {1.0f, 1.0f, 1.0f};
    // Alpha goes from 0 to 1 in half the time
    float alpha =
        MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
    Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         glm::vec4(tint, alpha));
    float yOffset = 0;
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.72f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -720.0f, 0.0f,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }
      MainItems->MoveTo(glm::vec2(0, yOffset));
      MainItems->Tint = glm::vec4(tint, 1.0f);
      MainItems->Render();
    }
  }
}

inline void SystemMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * FadeInDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
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

inline void SystemMenu::DrawErin() {
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

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto