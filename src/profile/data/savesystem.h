#pragma once

#include "../../data/savesystem.h"
#include <string>

namespace Impacto {
namespace Profile {
namespace SaveSystem {

int constexpr MaxAlbumEntries = 200;
int constexpr MaxAlbumSubEntries = 15;
int constexpr MaxCGSprites = 4;

inline Impacto::SaveSystem::SaveDataType Type =
    Impacto::SaveSystem::SaveDataType::None;

inline std::string SaveFilePath;
inline uint32_t* StoryScriptIDs;
inline int StoryScriptCount;
inline Impacto::SaveSystem::ScriptMessageDataPair* ScriptMessageData;
inline uint16_t AlbumEvData[MaxAlbumEntries][MaxAlbumSubEntries];
inline uint16_t AlbumData[MaxAlbumEntries][MaxAlbumSubEntries][MaxCGSprites];

void Configure();

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto