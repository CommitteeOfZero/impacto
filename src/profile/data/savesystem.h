#pragma once

#include "../../data/savesystem.h"
#include <string>

namespace Impacto {
namespace Profile {
namespace SaveSystem {

extern Impacto::SaveSystem::SaveDataType Type;

extern std::string SaveFilePath;

void Configure();

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto