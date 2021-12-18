#pragma once

#include "../../data/savesystem.h"
#include <string>

namespace Impacto {
namespace Profile {
namespace SaveSystem {

int const MaxAlbumEntries = 200;
int const MaxAlbumSubEntries = 15;
int const MaxCGSprites = 4;

extern Impacto::SaveSystem::SaveDataType Type;

extern std::string SaveFilePath;
extern uint32_t* StoryScriptIDs;
extern int StoryScriptCount;
extern Impacto::SaveSystem::ScriptMessageDataPair* ScriptMessageData;
extern uint16_t AlbumEvData[MaxAlbumEntries][MaxAlbumSubEntries];
extern uint16_t AlbumData[MaxAlbumEntries][MaxAlbumSubEntries][MaxCGSprites];

void Configure();

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto