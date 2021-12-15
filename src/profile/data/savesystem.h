#pragma once

#include "../../data/savesystem.h"
#include <string>

namespace Impacto {
namespace Profile {
namespace SaveSystem {

int const MaxAlbumEntries = 200;
int const MaxAlbumSubEntries = 15;

extern Impacto::SaveSystem::SaveDataType Type;

extern std::string SaveFilePath;
extern uint32_t* StoryScriptIDs;
extern int StoryScriptCount;
extern Impacto::SaveSystem::ScriptMessageDataPair* ScriptMessageData;
extern uint16_t AlbumData[MaxAlbumEntries][MaxAlbumSubEntries];

void Configure();

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto