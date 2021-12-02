#pragma once

#include "../../data/savesystem.h"
#include <string>

namespace Impacto {
namespace Profile {
namespace SaveSystem {

extern Impacto::SaveSystem::SaveDataType Type;

extern std::string SaveFilePath;
extern uint32_t* StoryScriptIDs;
extern int StoryScriptCount;
extern Impacto::SaveSystem::ScriptMessageDataPair* ScriptMessageData;

void Configure();

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto