#pragma once

#include <string>
#include <ankerl/unordered_dense.h>
#include "../util.h"

namespace Impacto::Profile {

struct GameDefinition {
  std::string GameProfile;
  ankerl::unordered_dense::map<std::string, std::string, string_hash,
                               std::equal_to<>>
      Patch;

  static void Configure();
};

inline ankerl::unordered_dense::map<std::string, GameDefinition, string_hash,
                                    std::equal_to<>>
    GameDefinitions;
}  // namespace Impacto::Profile