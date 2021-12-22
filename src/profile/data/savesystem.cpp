#include "savesystem.h"
#include "../profile_internal.h"
#include "../../window.h"

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
  EnsurePushMemberOfType("SaveData", kObjectType);

  Type = SaveDataType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  switch (Type) {
    case SaveDataType::CHLCC:
      Implementation = new Impacto::CHLCC::SaveSystem();
      break;
    case SaveDataType::MO6TW:
      Implementation = new Impacto::MO6TW::SaveSystem();
      break;
  }

  SaveFilePath = EnsureGetMemberString("SaveFilePath");

  if (TryPushMember("StoryScriptIDs")) {
    AssertIs(kArrayType);

    auto const& _storyScriptIds = TopVal();
    StoryScriptCount = _storyScriptIds.Size();
    StoryScriptIDs = (uint32_t*)malloc(StoryScriptCount * sizeof(uint32_t));
    for (uint32_t i = 0; i < StoryScriptCount; i++) {
      StoryScriptIDs[i] = EnsureGetArrayElementInt(i);
    }

    Pop();
  }

  if (TryPushMember("ScriptMessageData")) {
    AssertIs(kArrayType);

    auto const& _messageData = TopVal();
    auto dataCount = _messageData.Size();
    ScriptMessageData = new ScriptMessageDataPair[dataCount];
    for (uint32_t i = 0; i < dataCount; i++) {
      PushArrayElement(i);
      AssertIs(kArrayType);
      auto const& _pair = TopVal();
      if (_pair.Size() != 2) {
        ImpLog(LL_Fatal, LC_Profile, "Expected two values\n");
        Window::Shutdown();
      }
      ScriptMessageData[i].LineCount = EnsureGetArrayElementUint(0);
      ScriptMessageData[i].SaveDataOffset = EnsureGetArrayElementUint(1);
      Pop();
    }

    Pop();
  }

  if (TryPushMember("AlbumEvData")) {
    AssertIs(kArrayType);

    auto const& _albumEvData = TopVal();
    auto dataCount = _albumEvData.Size();
    for (uint32_t i = 0; i < dataCount; i++) {
      PushArrayElement(i);
      AssertIs(kArrayType);
      auto const& _albumSubData = TopVal();
      auto subDataCount = _albumSubData.Size();
      for (uint32_t j = 0; j < subDataCount; j++) {
        AlbumEvData[i][j] = EnsureGetArrayElementUint(j);
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
    AssertIs(kArrayType);

    auto const& _albumData = TopVal();
    auto dataCount = _albumData.Size();
    for (uint32_t i = 0; i < dataCount; i++) {
      PushArrayElement(i);
      AssertIs(kArrayType);
      auto const& _albumSubData = TopVal();
      auto subDataCount = _albumSubData.Size();
      for (uint32_t j = 0; j < subDataCount; j++) {
        PushArrayElement(j);
        AssertIs(kArrayType);
        auto const& _albumCGSprites = TopVal();
        auto cgSpriteCount = _albumCGSprites.Size();
        for (uint32_t k = 0; k < cgSpriteCount; k++) {
          AlbumData[i][j][k] = EnsureGetArrayElementUint(k);
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