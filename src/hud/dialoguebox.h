#pragma once

#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <ankerl/unordered_dense.h>

#include "../text/text.h"
#include "../ui/widgets/button.h"
#include "nametagdisplay.h"

namespace Impacto {

enum DialoguePageMode : uint8_t;
struct DialoguePage;

enum class DialogueBoxType : int {
  None,
  Plain,
  MO6TW,
  CHLCC,
  CC,
};
class DialogueBox {
 public:
  static std::unique_ptr<DialogueBox> Create(const DialoguePage& page);
  virtual ~DialogueBox() = default;

  virtual void Show() { ShowName(); }
  virtual void Hide() { HideName(); }
  void ShowName() { NametagDisplayInst->Show(); }
  void HideName() { NametagDisplayInst->Hide(); }

  virtual void Update(float dt) { NametagDisplayInst->Update(dt); }
  virtual void UpdateInput(float dt) {}

  virtual void Render(DialoguePageMode mode, const NameInfo& nameInfo,
                      glm::vec4 tint = glm::vec4(1.0f)) = 0;

  glm::vec2 GetScrWorkPos() const;

  enum class VisibilityStateType { Hidden, Hiding, Showing, Shown };
  using enum VisibilityStateType;
  VisibilityStateType VisibilityState = Hidden;

 protected:
  DialogueBox(const DialoguePage& page) : ParentPage(page) {}

  std::unique_ptr<NametagDisplay> NametagDisplayInst = NametagDisplay::Create();

  std::reference_wrapper<const DialoguePage> ParentPage;
};

class PlainDialogueBox : public DialogueBox {
 public:
  PlainDialogueBox(const DialoguePage& page);

  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Render(DialoguePageMode mode, const NameInfo& nameInfo,
              glm::vec4 tint) override;

 private:
  std::vector<std::unique_ptr<UI::Widgets::Button>> ControlButtons;

  void UpdateControlButtons(float dt);
  void RenderControlButtons(glm::vec4 col);
};

class VoidDialogueBox : public DialogueBox {
 public:
  VoidDialogueBox(const DialoguePage& page) : DialogueBox(page) {}

  void Show() override;
  void Hide() override;
  void Render(DialoguePageMode mode, const NameInfo& nameInfo,
              glm::vec4 tint) override {}
};

}  // namespace Impacto
