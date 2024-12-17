#pragma once

#include "../../ui/menu.h"
#include "../../spriteanimation.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/cclcc/optionsentry.h"

using namespace Impacto::UI::Widgets::CCLCC;

namespace Impacto {
namespace UI {
namespace CCLCC {

class OptionsMenu : public Menu {
 public:
  OptionsMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;

 private:
  void GoToPage(int pageNumber);

  Animation FadeAnimation;
  SpriteAnimation PoleAnimation;

  Widgets::Group* BasicPage;
  Widgets::Group* TextPage;
  Widgets::Group* SoundPage;
  Widgets::Group* VoicePage;

  int CurrentPage;
  std::vector<UI::Widgets::Group*> Pages;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto