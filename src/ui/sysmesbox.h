#pragma once

#include "menu.h"
#include "../vm/vm.h"
#include "../text/text.h"
#include "../ui/widgets/group.h"
#include "../game.h"

#include <map>
#include <memory>
#include <vector>

namespace Impacto {
namespace UI {

class SysMesBox : public Menu {
 public:
  virtual ~SysMesBox() = default;

  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void Render() override;

  virtual void Init() override;
  virtual void AddMessage(Vm::BufferOffsetContext str);
  virtual void AddChoice(Vm::BufferOffsetContext str);

  int MessageCount;
  int ChoiceCount;

  using SysMesBoxCreator = auto (*)() -> SysMesBox*;
  static inline SysMesBoxCreator Factory = nullptr;
  static inline Game::DrawComponentType DrawType =
      Game::DrawComponentType::None;
  static inline std::map<uint32_t, std::unique_ptr<SysMesBox>> MsgBoxes;

  static SysMesBox* Current(uint32_t id);
  static SysMesBox* Push(uint32_t id);
  static void Pop(uint32_t id);

 protected:
  std::unique_ptr<Widgets::Group> MessageItems;
  std::unique_ptr<Widgets::Group> ChoiceItems;

  float BoxOpacity;
  std::array<std::vector<ProcessedTextGlyph>, 8> Messages;
  std::array<float, 8> MessageWidths;
  std::array<std::vector<ProcessedTextGlyph>, 8> Choices;
  std::array<float, 8> ChoiceWidths;
  float ChoiceX = 0.0f;

  Animation FadeAnimation;
};

}  // namespace UI
}  // namespace Impacto