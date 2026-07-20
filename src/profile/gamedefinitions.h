#pragma once

#include <string>
#include <ankerl/unordered_dense.h>

namespace Impacto::Profile {

struct GameDefinition {
  std::string GameProfile;
  ankerl::unordered_dense::map<std::string, std::string> Patch;

  static void Configure();
};

inline ankerl::unordered_dense::map<std::string, GameDefinition>
    GameDefinitions;
}  // namespace Impacto::Profile