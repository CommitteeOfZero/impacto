#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class SaveEntryButton : public Widgets::Button {
 public:
  SaveEntryButton(int id, Sprite const& norm, Sprite const& focused,
                  Sprite const& highlight, glm::vec2 pos)
      : Widgets::Button(id, norm, focused, highlight, pos) {}
  void Render() override;
  void AddPlayTimeHintText(Vm::BufferOffsetContext strAdr, float fontSize,
                           bool outline);
  void AddPlayTimeText(Vm::BufferOffsetContext strAdr, float fontSize,
                       bool outline);
  void AddSaveDateHintText(Vm::BufferOffsetContext strAdr, float fontSize,
                           bool outline);
  void AddSaveDateText(Vm::BufferOffsetContext strAdr, float fontSize,
                       bool outline);
  void AddSceneTitleText(Vm::BufferOffsetContext strAdr, float fontSize,
                         bool outline);

  Sprite Thumbnail;

  bool EntryActive = false;

 private:
  std::vector<ProcessedTextGlyph> SceneTitle;
  std::vector<ProcessedTextGlyph> PlayTimeHint;
  std::vector<ProcessedTextGlyph> PlayTime;
  std::vector<ProcessedTextGlyph> SaveDateHint;
  std::vector<ProcessedTextGlyph> SaveDate;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto