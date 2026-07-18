#pragma once

namespace Impacto {
namespace Profile {

inline std::string BasePathsPath;
inline std::string GameDefinitionsPath;
inline std::string UserConfigPath;

void Init();
void Configure();
void ClearProfile();

}  // namespace Profile
}  // namespace Impacto