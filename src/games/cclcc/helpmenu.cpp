#include "helpmenu.h"
#include "../../renderer/renderer.h"
#include "../../profile/games/cclcc/helpmenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

HelpMenu::HelpMenu() {
  HelpItems = new Widgets::Group(this);

  // Load help images
  for (int i = 0; i < 17; i++) {
    Sprite image;
    // Assuming the images are named "help_0.png", "help_1.png", etc.
    image.Load("manual.mpk", "help_" + std::to_string(i) + ".png");
    HelpImages.push_back(image);
  }

void HelpMenu::Show() { 
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}

void HelpMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (ScrWork[SW_SYSSUBMENUCT] != 0) {
      TransitionAnimation.StartOut();
    } else {
      TransitionAnimation.Progress = 0.0f;
    }
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
    } else {
      UI::FocusedMenu = 0;
    }
  }
}

void HelpMenu::Update(float dt) {
  // UpdateInput();
  FadeAnimation.Update(dt);
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown &&
      ScrWork[SW_SYSSUBMENUNO] == 6) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] >= 32 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 6) {
    Show();
  }
}

void HelpMenu::Render() {
  Renderer->DrawSprite(HelpImages[CurrentImageIndex], glm::vec2(0.0f, 0.0f));
  HelpItems->Render();
}


void HelpMenu::UpdateInput() {
  HelpItems->UpdateInput();
  if (PADinputButtonWentDown & PAD1L) {
    PreviousImage();
  }
  if (PADinputButtonWentDown & PAD1R) {
    NextImage();
  }
}

void HelpMenu::NextImage() {
  CurrentImageIndex = (CurrentImageIndex + 1) % HelpImages.size();
  ImageTransitionAnimation.StartIn();
}

void HelpMenu::PreviousImage() {
  CurrentImageIndex =
      (CurrentImageIndex - 1 + HelpImages.size()) % HelpImages.size();
  ImageTransitionAnimation.StartIn();
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto