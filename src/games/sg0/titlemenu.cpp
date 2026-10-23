#include "titlemenu.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/sg0/titlemenu.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../games/cc/titlemenu.h"  // TitleMenuMode::PS4

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::SG0::TitleMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

// SW_TITLECUR1 values the SG0 title script dispatches on.
constexpr int MainItemIds[] = {0, -1, -1, 30, 40};
constexpr int SubItemIds[SubItemsNum] = {10, 11, 20, 21, 22, 23, 24};

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  ScrWork[SW_TITLECUR1] = target->Id;
  SetFlag(SF_TITLEEND, 1);
  AllowsScriptInput = true;
  if (CurrentSubMenu) {
    CurrentSubMenu->Hide();
    CurrentSubMenu = nullptr;
  }
}

void TitleMenu::OpenSubMenu(Widgets::Group* subMenu, Widgets::Button* parent) {
  parent->Hovered = false;
  CurrentSubMenu = subMenu;
  subMenu->Show();
  CurrentlyFocusedElement = subMenu->GetFirstFocusableChild();
  if (CurrentlyFocusedElement) CurrentlyFocusedElement->HasFocus = true;
  AllowsScriptInput = false;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  ContinueItems = new Widgets::Group(this);
  ContinueItems->WrapFocus = false;
  ExtraItems = new Widgets::Group(this);
  ExtraItems->WrapFocus = false;

  auto onClick = [this](Widgets::Button* btn) { MenuButtonOnClick(btn); };
  const Sprite nullSprite{};

  Widgets::Button* mainButtons[5];
  for (int i = 0; i < 5; i++) {
    const glm::vec2 pos = ItemPos + glm::vec2(0.0f, i * ItemSpacing);
    mainButtons[i] =
        new Widgets::Button(MainItemIds[i], MenuEntriesSprites[i],
                            MenuEntriesHSprites[i], nullSprite, pos);
    mainButtons[i]->OnClickHandler = onClick;
    MainItems->Add(mainButtons[i], FDIR_DOWN);
  }
  NewGame = mainButtons[0];
  mainButtons[1]->OnClickHandler = [this](Widgets::Button* btn) {
    OpenSubMenu(ContinueItems, btn);
  };
  mainButtons[2]->OnClickHandler = [this](Widgets::Button* btn) {
    OpenSubMenu(ExtraItems, btn);
  };

  // Sub items hang to the right of their parent item.
  for (int i = 0; i < SubItemsNum; i++) {
    const bool isContinue = i < 2;
    const int parentIndex = isContinue ? 1 : 2;
    const int row = isContinue ? i : i - 2;
    const glm::vec2 pos =
        ItemPos + glm::vec2(SubItemOffsetX,
                            parentIndex * ItemSpacing + row * SubItemSpacing);
    auto* button = new Widgets::Button(SubItemIds[i], SubItemSprites[i],
                                       SubItemHSprites[i], nullSprite, pos);
    button->OnClickHandler = onClick;
    (isContinue ? ContinueItems : ExtraItems)->Add(button, FDIR_DOWN);
  }
}

void TitleMenu::Show() {
  if (State == Shown) return;
  State = Shown;
  if (UI::FocusedMenu != nullptr) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  IsFocused = true;
  UI::FocusedMenu = this;
  AllowsScriptInput = true;
  if (PressToStartAnimation.State == AnimationState::Stopped) {
    PressToStartAnimation.StartIn();
  }
}

void TitleMenu::Hide() {
  if (State == Hidden) return;
  State = Hidden;
  MainItems->Hide();
  ContinueItems->Hide();
  ExtraItems->Hide();
  CurrentSubMenu = nullptr;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
  } else {
    UI::FocusedMenu = nullptr;
  }
  IsFocused = false;
  AllowsScriptInput = true;
}

void TitleMenu::UpdateInput(float dt) {
  // An EXTRA screen, TIPS or the config opened from here holds the focus;
  // their key presses must not also click the title's items.
  if (!IsFocused) return;
  Menu::UpdateInput(dt);
  if (CurrentSubMenu) {
    CurrentSubMenu->UpdateInput(dt);
    if (PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) {
      // Back out of the sub menu to the main items.
      CurrentSubMenu->Hide();
      CurrentSubMenu = nullptr;
      AllowsScriptInput = true;
    }
  } else {
    MainItems->UpdateInput(dt);
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput(dt);
  PressToStartAnimation.Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }
  if (State == Hidden) return;

  using namespace CC::TitleMenuMode::PS4;
  switch (ScrWork[SW_TITLEMODE]) {
    case PressToStart:
      PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
      PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
      break;
    case StartTransition:
      PressToStartAnimation.DurationIn = PressToStartAnimFastDurationIn;
      PressToStartAnimation.DurationOut = PressToStartAnimFastDurationOut;
      break;
    case Main:
      MainItems->Update(dt);
      if (CurrentSubMenu) CurrentSubMenu->Update(dt);
      if (MainItems->VisibilityState == Hidden) {
        MainItems->Show();
        CurrentlyFocusedElement = NewGame;
        NewGame->HasFocus = true;
      }
      break;
  }
}

void TitleMenu::Render() {
  if (State == Hidden || !GetFlag(SF_TITLEMODE)) return;

  using namespace CC::TitleMenuMode::PS4;
  switch (ScrWork[SW_TITLEMODE]) {
    case PressToStart:
    case StartTransition:
      DrawLogo(1.0f);
      DrawStartButton();
      break;
    case Main:
      DrawLogo(1.0f);
      MainItems->Render();
      if (CurrentSubMenu) CurrentSubMenu->Render();
      break;
    case InitialFade:
      DrawLogo(ScrWork[SW_TITLEDISPCT] / 32.0f);
      break;
  }
}

void TitleMenu::DrawLogo(float opacity) {
  Renderer->DrawSprite(
      LogoSprite, LogoPos,
      glm::vec4(1.0f, 1.0f, 1.0f, std::clamp(opacity, 0.0f, 1.0f)));
}

void TitleMenu::DrawStartButton() {
  glm::vec4 col(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
  Renderer->DrawSprite(PressToStartSprite, PressToStartPos, col);
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
