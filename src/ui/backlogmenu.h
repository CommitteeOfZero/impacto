#pragma once

#include "menu.h"
#include "widgets/group.h"
#include "widgets/backlogentry.h"
#include "widgets/scrollbar.h"

namespace Impacto {
namespace UI {

class BacklogMenuInterface : public Menu {
 public:
  virtual void AddMessage(Vm::BufferOffsetContext scrCtx,
                          std::optional<int> audioId = std::nullopt,
                          int characterId = 0) = 0;

  virtual void MenuButtonOnClick(Widgets::BacklogEntry* target) = 0;
  virtual void Clear() = 0;

  float PageY = 0.0f;
};

template <typename EntryType>
class BacklogMenu : public BacklogMenuInterface {
  static_assert(std::derived_from<EntryType, Widgets::BacklogEntry>);

 public:
  BacklogMenu();

  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void UpdateInput(float dt) override;
  virtual void Render() override;

  void AddMessage(Vm::BufferOffsetContext scrCtx,
                  std::optional<int> audioId = std::nullopt,
                  int characterId = 0) override;

  virtual void MenuButtonOnClick(Widgets::BacklogEntry* target) override;
  void Clear() override;

 protected:
  virtual void UpdateVisibility() {};

  boost::circular_buffer<EntryType> Entries;

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