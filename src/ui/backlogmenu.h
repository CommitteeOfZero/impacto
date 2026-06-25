#pragma once

#include "menu.h"
#include "widgets/group.h"
#include "widgets/backlogentry.h"
#include "widgets/scrollbar.h"

namespace Impacto {
namespace UI {

class BacklogMenu : public Menu {
 public:
  BacklogMenu();

  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void UpdateInput(float dt) override;
  virtual void Render() override;

  virtual Widgets::BacklogEntry* CreateBacklogEntry(
      Vm::BufferOffsetContext scrCtx, std::optional<int> audioId,
      int characterId, glm::vec2 pos, const RectF& hoverBounds) const {
    return new Widgets::BacklogEntry(scrCtx, audioId, characterId, pos,
                                     hoverBounds);
  }

  virtual void AddMessage(Vm::BufferOffsetContext scrCtx,
                          std::optional<int> audioId = std::nullopt,
                          int characterId = 0);

  virtual void MenuButtonOnClick(Widgets::BacklogEntry* target);
  void Clear();

  float PageY = 0.0f;

 protected:
  virtual void UpdateVisibility() {};

  boost::circular_buffer<std::unique_ptr<Widgets::BacklogEntry>> Entries;

  float ItemsHeight = 0.0f;
  Animation FadeAnimation;
  Widgets::Scrollbar MainScrollbar;

  void RenderHighlight(glm::vec2 offset = {0.0f, 0.0f}) const;
  void UpdatePageUpDownInput(float dt);
  void UpdateScrollingInput(float dt);

  void MoveEntriesBottomTo(float yPosition);

 private:
  float ScrollCloseTimer = 0.0f;
};

}  // namespace UI
}  // namespace Impacto