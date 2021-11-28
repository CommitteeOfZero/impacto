#pragma once

#include "menu.h"
#include "../text.h"
#include "../ui/widgets/group.h"
#include "../ui/widgets/label.h"

namespace Impacto {
namespace UI {

class TipsMenu : public Menu {
 public:
  virtual void Init();
  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt);
  virtual void Render();

 protected:
  virtual void SwitchToTipId(int id);
  virtual void NextTipPage();

  int CurrentlyDisplayedTipId = -1;

  Animation FadeAnimation;
  DialoguePage* TextPage;
  Widgets::Label* Name;
  Widgets::Label* Pronounciation;
  Widgets::Label* Category;
  Widgets::Label* NumberText;
  Widgets::Label* Number;
};

}  // namespace UI
}  // namespace Impacto