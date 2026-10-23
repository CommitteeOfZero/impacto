#pragma once

#include <array>
#include <vector>

#include "../../ui/savemenu.h"
#include "../../text/text.h"
#include "../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 save / load / quick-load screen. The scripts drive it like
// CHAOS;CHILD Love Chu Chu's: SaveMenu(0, type) selects the page type,
// SW_SYSSUBMENUNO (0 load, 3 save, 4 quick load) and SW_SYSSUBMENUCT (counting
// up to SubMenuCountMax) open and close it, and a chosen slot is reported
// through SW_SAVEFILENO / SW_SAVEFILETYPE plus ChoiceMade.
class SaveMenu : public UI::SaveMenu {
 public:
  SaveMenu();
  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;
  void RefreshCurrentEntryInfo() override;

 private:
  struct EntryInfo {
    uint8_t Status = 0;
    bool Locked = false;
    std::vector<ProcessedTextGlyph> PlayTime;
    std::vector<ProcessedTextGlyph> SaveDate;
  };

  SaveSystem::SaveType CurrentSaveType() const;
  void RefreshEntry(int id);
  void RefreshAllEntries();
  void Choose(int id);
  glm::vec2 EntryPos(int id) const;

  int Cursor = 0;
  int LastSubMenuCount = 0;
  std::array<EntryInfo, SaveSystem::MaxSaveEntries> Entries;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
