#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/scrollbar.h"
#include "../../ui/widgets/toggle.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class OptionsMenu : public Menu {
 public:
  OptionsMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  Animation FadeAnimation;

  Widgets::Scrollbar* VoiceVolumeSlider;
  Widgets::Scrollbar* BGMVolumeSlider;
  Widgets::Scrollbar* SEVolumeSlider;
  Widgets::Scrollbar* MovieVolumeSlider;
  Widgets::Group* FirstPage;
  Widgets::Group* CharacterVoiceToggles;
  bool VoiceTest[13];
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto