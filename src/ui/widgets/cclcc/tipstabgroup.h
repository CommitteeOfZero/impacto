#pragma once

#include "../button.h"
#include "../group.h"
#include "../../turboonholdhandler.h"
#include "../../../data/tipssystem.h"
#include "../../../games/cclcc/tipsmenu.h"
#include "tipsentrybutton.h"
#include <vector>
#include <memory>

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class TipsTabButton : public Widgets::Button {
 public:
  TipsTabButton(Impacto::UI::CCLCC::TipsTabType type,
                std::function<void(Widgets::Button*)> onClickHandler);
  void UpdateInput(float dt) override;
  void Reset();
};

class TipsTabGroup : public Menu {
 public:
  TipsTabGroup(Impacto::UI::CCLCC::TipsTabType type,
               std::function<void(Widgets::Button*)> tabClickHandler,
               std::function<void(Widgets::Button*)> tipClickHandler);

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdatePageInput(float dt);
  void Render() override;
  void UpdateTipsEntries(std::vector<int> const& SortedTipIds);
  size_t GetTipEntriesCount() { return TipsEntryButtons.size(); }
  void Move(glm::vec2 offset);
  void MoveTo(glm::vec2 pos);

  glm::vec4 Tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

 private:
  Widgets::Group TipsEntriesGroup;
  TipsTabButton TabName;
  Impacto::UI::CCLCC::TipsTabType Type;
  std::function<void(Widgets::Button*)> TipClickHandler;
  std::unique_ptr<Widgets::Scrollbar> TipsEntriesScrollbar;
  std::vector<TipsEntryButton*> TipsEntryButtons;
  float ScrollPosY = 0.0f;
  glm::vec2 TipsScrollStartPos;
  glm::vec2 TipsScrollTrackBounds;

  TurboOnHoldHandler DirectionButtonHoldHandler;
  TurboOnHoldHandler PageUpDownButtonHoldHandler;

  int EntriesPerPage;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto