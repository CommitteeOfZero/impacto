#pragma once

#include "menu.h"
#include "../text/text.h"
#include "../text/dialoguepage.h"
#include "../ui/widgets/group.h"
#include "../ui/widgets/label.h"

namespace Impacto {
namespace UI {

class TipsMenu : public Menu {
 public:
  TipsMenu() { InputConfig = InputRate::RepeatFast; }
  virtual void Init() override;
  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void Render() override;

 protected:
  virtual void SwitchToTipId(int id);
  virtual void NextTipPage();

  int CurrentlyDisplayedTipId = -1;

  Animation FadeAnimation;
  DialoguePage TextPage;
  Widgets::Label* Name;
  Widgets::Label* Pronounciation;
  Widgets::Label* Category;
  Widgets::Label* NumberText;
  Widgets::Label* Number;
};

}  // namespace UI
}  // namespace Impacto