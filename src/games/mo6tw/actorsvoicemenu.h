#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/mo6tw/actorsvoicebutton.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class ActorsVoiceMenu : public Menu {
 public:
  ActorsVoiceMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void VoiceButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;

  void UpdateActorsVoiceEntries();

  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto