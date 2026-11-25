#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <ankerl/unordered_dense.h>
#include "../util.h"

namespace Impacto::Profile {

struct PatchDefinitions {
  struct PatchEntry {
    std::string Name;
    std::string PatchPath;
  };

  auto begin() const { return Entries.cbegin(); }
  auto end() const { return Entries.cend(); }
  std::string const* Find(std::string_view name) const;
  bool Empty() const { return Entries.empty(); }

  std::vector<PatchEntry> Entries;
};
struct GameDefinition {
  std::string GameProfile;
  std::string Name;
  PatchDefinitions Patch;
  bool Hidden = false;
  int LauncherOrderId = 99;
  uint32_t LauncherTheme = 0xFFFFFF;

  static void Configure();
};

inline ankerl::unordered_dense::map<std::string, GameDefinition, string_hash,
                                    std::equal_to<>>
    GameDefinitions;
}  // namespace Impacto::Profile