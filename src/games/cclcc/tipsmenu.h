#pragma once

#include "../../ui/tipsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"
#include "../../data/tipssystem.h"
#include "../../ui/widgets/cclcc/tipsentrybutton.h"
#include "../../ui/widgets/scrollbar.h"
#include <vector>

namespace Impacto::UI::Widgets::CCLCC {
class TipsTabGroup;
}
namespace Impacto {
namespace UI {
namespace CCLCC {

enum TipsTabType {
  AllTips,
  UnlockedTips,
  UnreadTips,
  NewTips,
};

class TipsMenu : public UI::TipsMenu {
 public:
  TipsMenu();

  void Init();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void TipOnClick(Widgets::Button* target);

 protected:
  void SwitchToTipId(int id);
  void NextTipPage();

 private:
  constexpr int static TabCount = 4;
  Animation DelayAnimation;
  int CurrentTabIdx = 0;
  float TipPageY = 0;
  glm::vec2 TipsScrollStartPos;
  glm::vec2 TipsScrollTrackBounds;

  TipsTabType CurrentTabType;
  std::vector<int> SortedTipIds;
  std::array<Widgets::CCLCC::TipsTabGroup*, TabCount> TipsTabs;

  Widgets::Scrollbar* TipsScrollbar = nullptr;
  Widgets::Group TipViewItems;

  int ScrollWheelYDelta = 0;
  bool MouseInTextBounds = false;

  float ScrollPercentage = 0.0f;
  bool HasInitialized = false;
  void SetActiveTab(TipsTabType type);

  float LastYPos = 0.0f;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto