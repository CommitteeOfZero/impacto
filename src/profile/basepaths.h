#pragma once

#include <string>

namespace Impacto::Profile::BasePaths {
inline std::string RootGamedataDir;
inline std::string RootProfilesDir;
inline std::string RootPatchesDir;
inline std::string RootSavesDir;

void Configure();
}  // namespace Impacto::Profile::BasePaths