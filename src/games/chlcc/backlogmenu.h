#pragma once

#include "commonmenu.h"
#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/chlcc/backlogentry.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class BacklogMenu : public UI::BacklogMenu<Widgets::CHLCC::BacklogEntry>,
                    public CommonMenu {
 public:
  BacklogMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto