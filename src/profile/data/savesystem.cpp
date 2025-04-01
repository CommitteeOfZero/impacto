#include "savesystem.h"
#include "../profile_internal.h"
#include "../../renderer/renderer.h"

#include "../../games/cclcc/savesystem.h"
#include "../../games/chlcc/savesystem.h"
#include "../../games/mo6tw/savesystem.h"

namespace Impacto {
namespace Profile {
namespace SaveSystem {

using namespace Impacto::SaveSystem;

void Configure() {
  EnsurePushMemberOfType("SaveData", LUA_TTABLE);

  Type = SaveDataType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

  switch (Type) {
    case SaveDataType::CHLCC:
      Implementation = new Impacto::CHLCC::SaveSystem();
      break;
    case SaveDataType::MO6TW:
      Implementation = new Impacto::MO6TW::SaveSystem();
      break;
    case SaveDataType::CCLCC:
      Implementation = new Impacto::CCLCC::SaveSystem();
      break;
    case SaveDataType::None:
      ImpLog(LogLevel::Warning, LogChannel::Profile,
             "Save data type is none, not setting implementation\n");
      break;
  }

  SaveFilePath = EnsureGetMember<std::string>("SaveFilePath");

  if (TryPushMember("StoryScriptIDs")) {
    AssertIs(LUA_TTABLE);

    StoryScriptCount = (int)lua_rawlen(LuaState, -1);
    StoryScriptIDs.resize(*StoryScriptCount);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKey<int32_t>() - 1;
      StoryScriptIDs[i] = EnsureGetArrayElement<int>();
      Pop();
    }

    Pop();
  }

  if (TryPushMember("ScriptMessageData")) {
    AssertIs(LUA_TTABLE);

    auto dataCount = lua_rawlen(LuaState, -1);
    ScriptMessageData.resize(dataCount);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKey<int32_t>() - 1;
      AssertIs(LUA_TTABLE);
      auto pairSize = lua_rawlen(LuaState, -1);
      if (pairSize != 2) {
        ImpLog(LogLevel::Fatal, LogChannel::Profile, "Expected two values\n");
        Window->Shutdown();
      }
      ScriptMessageData[i].LineCount =
          EnsureGetArrayElementByIndex<uint32_t>(0);
      ScriptMessageData[i].SaveDataOffset =
          EnsureGetArrayElementByIndex<uint32_t>(1);

      Pop();
    }

    Pop();
  }

  if (TryPushMember("AlbumEvData")) {
    AssertIs(LUA_TTABLE);

    auto dataCount = lua_rawlen(LuaState, -1);
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKey<int32_t>() - 1;
      AssertIs(LUA_TTABLE);
      auto subDataCount = lua_rawlen(LuaState, -1);
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        int j = EnsureGetKey<int32_t>() - 1;
        AlbumEvData[i][j] = EnsureGetArrayElement<uint32_t>();
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
      int i = EnsureGetKey<int32_t>() - 1;
      AssertIs(LUA_TTABLE);
      auto subDataCount = lua_rawlen(LuaState, -1);
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        int j = EnsureGetKey<int32_t>() - 1;
        AssertIs(LUA_TTABLE);
        auto cgSpriteCount = lua_rawlen(LuaState, -1);
        PushInitialIndex();
        while (PushNextTableElement() != 0) {
          int k = EnsureGetKey<int32_t>() - 1;
          AlbumData[i][j][k] = EnsureGetArrayElement<uint32_t>();
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