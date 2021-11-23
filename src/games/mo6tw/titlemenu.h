#pragma once

#include "../../animation.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/mo6tw/titlebutton.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  Animation PressToStartAnimation;
  Animation PrimaryFadeAnimation;
  Animation SecondaryFadeAnimation;

  void MenuButtonOnClick(Widgets::Button* target);
  void SecondaryButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;
  Widgets::MO6TW::TitleButton* InitialStart;
  Widgets::MO6TW::TitleButton* ExtraStory;
  Widgets::MO6TW::TitleButton* Continue;
  Widgets::MO6TW::TitleButton* Memories;
  Widgets::MO6TW::TitleButton* Encyclopedia;
  Widgets::MO6TW::TitleButton* System;
  Widgets::MO6TW::TitleButton* Exit;

  Widgets::Group* ExtraStoryItems;
  Widgets::MO6TW::TitleButton* Prologue;
  Widgets::MO6TW::TitleButton* OtomeJudgeShin;
  Widgets::MO6TW::TitleButton* Warning;
  void ShowExtraStoryItems();
  void HideExtraStoryItems();

  Widgets::Group* ContinueItems;
  Widgets::MO6TW::TitleButton* Load;
  Widgets::MO6TW::TitleButton* QuickLoad;
  void ShowContinueItems();
  void HideContinueItems();

  Widgets::Group* MemoriesItems;
  Widgets::MO6TW::TitleButton* ClearList;
  Widgets::MO6TW::TitleButton* Album;
  Widgets::MO6TW::TitleButton* Music;
  Widgets::MO6TW::TitleButton* Movie;
  Widgets::MO6TW::TitleButton* ActorsVoice;
  void ShowMemoriesItems();
  void HideMemoriesItems();

  Widgets::Group* SystemItems;
  Widgets::MO6TW::TitleButton* Option;
  Widgets::MO6TW::TitleButton* SystemSave;
  void ShowSystemItems();
  void HideSystemItems();
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto