#pragma once

#include "animations/selectprompt.h"
#include "animations/menutransition.h"
#include "../../ui/tipsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/clickarea.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/scrollbar.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class TipsMenu : public UI::TipsMenu {
 public:
  TipsMenu();

  void Init() override;

  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;
  void UpdatePageInput(float dt);

  void TipOnClick(Widgets::Button* target);

 protected:
  void SwitchToTipId(int id) override;
  void AdvanceTipPage(TipAdvanceMode mode) override;

 private:
  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawTipsTree();
  void DrawButtonPrompt();
  void UpdateTitles();
  void HandlePageChange(Widget* cur, Widget* next);

  Widgets::ClickArea PrevPageTipClickArea;
  Widgets::ClickArea NextPageTipClickArea;
  Animation TitleFade;
  Animation FromSystemMenuTransition;
  SelectPromptAnimation SelectAnimation;
  MenuTransitionAnimation MenuTransition;
  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;

  int CurrentTipPage = 1;
  float TipsEntryScrollPos = 0;

  Widgets::Carousel ItemsList;
  Widgets::Group TipViewItems;
  Widgets::Label* CurrentPage;
  Widgets::Label* TotalPages;
  std::vector<uint16_t> CategoryStringBuffer;
  std::optional<Widgets::Scrollbar> TipsEntriesScrollbar;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto