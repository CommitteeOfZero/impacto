#pragma once

#include <array>
#include <vector>

#include "../../ui/tipsmenu.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 TIPS (sysm_tips.dds, tipschips.dds): a tabbed list of tips on
// the left (ALL / ACQUIRED / UNREAD / NEW) and the chosen tip on the right.
// _Startup_win opens it with SW_SYSSUBMENUNO 2 and SW_SYSSUBMENUCT counting
// 0..32, runs Tips(2) every frame until Back, then counts down.
class TipsMenu : public UI::TipsMenu {
 public:
  void Init() override;
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

 private:
  enum Tab { All, Acquired, Unread, New, TabCount };
  struct ListItem {
    int TipId;
    std::vector<ProcessedTextGlyph> Text;
  };

  void BuildList();
  void SetTab(int tab);
  void ShowTip(int tipId);
  void ClampListScroll();

  int CurrentTab = All;
  std::array<int, TabCount> TabCounts{};
  std::vector<ListItem> Items;
  int Cursor = 0;
  int ListScroll = 0;  // first visible row

  int DisplayedTip = -1;
  std::vector<ProcessedTextGlyph> NumberGlyphs;
  std::vector<ProcessedTextGlyph> CategoryGlyphs;
  std::vector<ProcessedTextGlyph> NameGlyphs;
  float NoteScroll = 0.0f;
  float NoteScrollMax = 0.0f;
  int LastSubMenuCount = 0;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
