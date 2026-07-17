#pragma once

#include "../cclcc/commonmenu.h"
#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/cc/backlogentry.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CC {

class BacklogMenu : public UI::BacklogMenu<Widgets::CC::BacklogEntry>,
                    public CCLCC::CommonMenu {
 public:
  void Show() override;
  void Hide() override;
  void Render() override;
  void Update(float dt) override;

  void MenuButtonOnClick(Widgets::BacklogEntry* target) override;

 private:
  void UpdateVisibility() override;
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto