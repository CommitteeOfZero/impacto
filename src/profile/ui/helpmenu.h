#pragma once

#include <string>
#include <vector>

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace HelpMenu {

inline Impacto::UI::HelpMenuType Type = Impacto::UI::HelpMenuType::None;

inline bool UseOverlayHelp = false;
inline int OverlayHelpKbCustomId = -1;

struct HelpAction {
  std::string Section;
  std::string Action;
  std::string KeyboardMouse;
  std::string Controller;
};
inline std::vector<HelpAction> Actions;

void Configure();

}  // namespace HelpMenu
}  // namespace Profile
}  // namespace Impacto