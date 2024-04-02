#include "savesystem.h"
#include "../profile_internal.h"
#include "../../renderer/renderer.h"

#include "../../games/chlcc/savesystem.h"
#include "../../games/mo6tw/savesystem.h"

namespace Impacto {
namespace Profile {
namespace SaveSystem {

using namespace Impacto::SaveSystem;

SaveDataType Type = SaveDataType::None;

std::string SaveFilePath;
uint32_t* StoryScriptIDs;
int StoryScriptCount;
ScriptMessageDataPair* ScriptMessageData;
uint16_t AlbumEvData[MaxAlbumEntries][MaxAlbumSubEntries];
uint16_t AlbumData[MaxAlbumEntries][MaxAlbumSubEntries][MaxCGSprites];

void Configure() {
  EnsurePushMemberOfType("SaveData", LUA_TTABLE);

  Type = SaveDataType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  switch (Type) {
    case SaveDataType::CHLCC:
      Implementation = new Impacto::CHLCC::SaveSystem();
      break;
    case SaveDataType::MO6TW:
      Implementation = new Impacto::MO6TW::SaveSystem();
      break;
    case SaveDataType::None:
      ImpLog(LL_Warning, LC_Profile,
             "Save data type is none, not setting implementation\n");
      break;
  }

  SaveFilePath = EnsureGetMemberString("SaveFilePath");

  if (TryPushMember("StoryScriptIDs")) {
    AssertIs(LUA_TTABLE);

    StoryScriptCount = (int)lua_rawlen(LuaState, -1);
    StoryScriptIDs = (uint32_t*)malloc(StoryScriptCount * sizeof(uint32_t));
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKeyInt() - 1;
      StoryScriptIDs[i] = EnsureGetArrayElementInt();
      Pop();
    }

    Pop();
  }

  if (TryPushMember("ScriptMessageData")) {
    AssertIs(LUA_TTABLE);

    auto dataCount = lua_rawlen(LuaState, -1);
    ScriptMessageData = new ScriptMessageDataPair[dataCount];
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKeyInt() - 1;
      AssertIs(LUA_TTABLE);
      auto pairSize = lua_rawlen(LuaState, -1);
      if (pairSize != 2) {
        ImpLog(LL_Fatal, LC_Profile, "Expected two values\n");
        Window->Shutdown();
      }
      ScriptMessageData[i].LineCount = EnsureGetArrayElementByIndexUint(0);
      ScriptMessageData[i].SaveDataOffset = EnsureGetArrayElementByIndexUint(1);

      Pop();
    }

    Pop();
  }

  if (TryPushMember("AlbumEvData")) {
    AssertIs(LUA_TTABLE);

    auto dataCount = lua_rawlen(LuaState, -1);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKeyInt() - 1;
      AssertIs(LUA_TTABLE);
      auto subDataCount = lua_rawlen(LuaState, -1);
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        int j = EnsureGetKeyInt() - 1;
        AlbumEvData[i][j] = EnsureGetArrayElementUint();
        Pop();
      }
      // End marker
      AlbumEvData[i][subDataCount] = 0xFFFF;
      Pop();
    }
    // End marker
    AlbumEvData[dataCount][0] = 0xFFFF;

    Pop();
  }

  if (TryPushMember("AlbumData")) {
    AssertIs(LUA_TTABLE);

    // Unused for now
    // auto dataCount = lua_rawlen(LuaState, -1);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKeyInt() - 1;
      AssertIs(LUA_TTABLE);
      auto subDataCount = lua_rawlen(LuaState, -1);
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        int j = EnsureGetKeyInt() - 1;
        AssertIs(LUA_TTABLE);
        auto cgSpriteCount = lua_rawlen(LuaState, -1);
        PushInitialIndex();
        while (PushNextTableElement() != 0) {
          int k = EnsureGetKeyInt() - 1;
          AlbumData[i][j][k] = EnsureGetArrayElementUint();
          Pop();
        }
        // End marker
        AlbumData[i][j][cgSpriteCount] = 0xFFFF;
        Pop();
      }
      // End marker
      AlbumData[i][subDataCount][0] = 0xFFFF;
      Pop();
    }
    Pop();
  }

  Pop();
}

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto