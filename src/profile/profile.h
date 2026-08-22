#pragma once

namespace Impacto {
namespace Profile {

inline std::string BasePathsPath;
inline std::string GameDefinitionsPath;

void Init();
void Configure();
void ConfigureGameProfile();
void ClearProfile();

}  // namespace Profile
}  // namespace Impacto