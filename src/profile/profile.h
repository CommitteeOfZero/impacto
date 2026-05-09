#pragma once

namespace Impacto {
namespace Profile {

inline std::string BaseConfigPath;
inline std::string UserConfigPath;

void Init();
void Configure();
void ClearProfile();

}  // namespace Profile
}  // namespace Impacto