#pragma once

#include <string>
#include <ankerl/unordered_dense.h>

namespace Impacto::Profile::BaseConfig {

struct GameDefinition {
  std::string GameProfile;
  ankerl::unordered_dense::map<std::string, std::string> Patch;
};

inline ankerl::unordered_dense::map<std::string, GameDefinition>
    GameDefinitions;
inline std::string RootGamedataDir;
inline std::string RootProfilesDir;
inline std::string RootPatchesDir;
inline std::string RootSavesDir;

void Configure();
std::string const& GetPlatformSpecificPath();
}  // namespace Impacto::Profile::BaseConfig