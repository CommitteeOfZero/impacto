#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../spritesheet.h"

namespace Impacto::Profile {

namespace Overlay::HelpMenu {

inline bool UseOverlayHelp = false;
inline std::optional<uint8_t> OverlayHelpKbCustomId;

struct HelpAction {
  std::string Action;
  std::string KeyboardMouse;
  std::string Controller;
  std::string Touch;
};

struct HelpSection {
  std::string Name;
  std::vector<HelpAction> Actions;
};
inline std::vector<HelpSection> Sections;

void Configure();

}  // namespace Overlay::HelpMenu

}  // namespace Impacto::Profile
