#include "savesystem.h"
#include "../profile.h"
#include "../profile_internal.h"
#include "../../renderer/renderer.h"

#include "../../games/cclcc/savesystem.h"
#include "../../games/chlcc/savesystem.h"
#include "../../games/mo6tw/savesystem.h"

namespace Impacto {
namespace Profile {

template <>
struct TryGetImpl<SaveSystem::AddedLinesDataStruct> {
  static std::optional<SaveSystem::AddedLinesDataStruct> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    const std::optional<size_t> bitFieldOffset =
        TryGetMember<size_t>("BitFieldOffset");
    if (!bitFieldOffset.has_value()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile, "Missing BitFieldOffset");
      return std::nullopt;
    }

    const std::optional<size_t> addedLinesPerScript =
        TryGetMember<size_t>("AddedLinesPerScript");
    if (!addedLinesPerScript.has_value()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Missing AddedLinesPerScript");
      return std::nullopt;
    }

    return SaveSystem::AddedLinesDataStruct{
        .BitFieldOffset = *bitFieldOffset,
        .AddedLinesPerScript = *addedLinesPerScript,
    };
  }
};

namespace SaveSystem {
struct LineRange {
  size_t ScriptId;
  size_t StartLineId;
  size_t EndLineId;

  size_t EquivalentLineRangesIdx = 0;

  bool Intersects(const LineRange& other) const {
    return ScriptId == other.ScriptId &&
           !(other.EndLineId < StartLineId || EndLineId < other.StartLineId);
  }
};
}  // namespace SaveSystem

template <>
struct TryGetImpl<SaveSystem::LineRange> {
  static std::optional<SaveSystem::LineRange> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    const std::optional<size_t> scriptId = TryGetMember<size_t>("ScriptId");
    if (!scriptId.has_value()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Missing ScriptId in equivalent line range");
      return std::nullopt;
    }

    std::optional<size_t> startLineId = TryGetMember<size_t>("StartLineId");
    std::optional<size_t> endLineId;
    if (!startLineId.has_value()) {
      startLineId = TryGetMember<size_t>("LineId");

      if (!startLineId.has_value()) {
        ImpLog(LogLevel::Fatal, LogChannel::Profile,
               "Missing LineId in equivalent line range");
        return std::nullopt;
      }

      endLineId = startLineId;
    } else {
      endLineId = TryGetMember<size_t>("EndLineId");

      if (!endLineId.has_value()) {
        ImpLog(LogLevel::Fatal, LogChannel::Profile,
               "Missing EndLineId in equivalent line range");
        return std::nullopt;
      }
    }

    return SaveSystem::LineRange{
        .ScriptId = *scriptId,
        .StartLineId = *startLineId,
        .EndLineId = *endLineId,
    };
  }
};

namespace SaveSystem {
using namespace Impacto::SaveSystem;

// EquivalentRanges holds all equivalent classes between ranges.
// ScriptRanges allows one to fetch all equivalent classes pertaining to an
// individual script ID, for faster lookups.
static std::vector<std::vector<LineRange>> EquivalentLineRanges;
static std::map<size_t, std::vector<LineRange>> ScriptLineRanges;

void Configure() {
  EnsurePushMemberOfType("SaveData", LUA_TTABLE);

  Type = EnsureGetMember<SaveDataType>("Type");

  switch (Type) {
    case SaveDataType::CHLCC:
      Implementation = new Impacto::CHLCC::SaveSystem();

      ThumbnailFilePath = EnsureGetMember<std::string>("ThumbnailFilePath");

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
        AlbumEvData[i][j] = EnsureGetArrayElement<uint16_t>();
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
          AlbumData[i][j][k] = EnsureGetArrayElement<uint16_t>();
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

  AddedLinesData = TryGetMember<AddedLinesDataStruct>("AddedLinesData");

  if (TryPushMember("EquivalentLines")) {
    ForEachProfileArray([](const uint32_t index) {
      std::vector<LineRange>& equivalentRangeList =
          EquivalentLineRanges.emplace_back(
              EnsureGet<std::vector<LineRange>>());
      if (equivalentRangeList.size() < 2) {
        ImpLog(LogLevel::Warning, LogChannel::Profile,
               "Equivalent range list of size < 2 loaded from the profile");
        EquivalentLineRanges.pop_back();
        return;
      }

      const size_t rangeLength = equivalentRangeList.front().EndLineId -
                                 equivalentRangeList.front().StartLineId;
      for (auto curIt = equivalentRangeList.begin();
           curIt != equivalentRangeList.end(); curIt++) {
        if (curIt->EndLineId < curIt->StartLineId) {
          throw std::runtime_error(fmt::format(
              "EndLineId < StartLineId for equivalent line range [{:d}, "
              "{:d}] of script {:d}",
              curIt->StartLineId, curIt->EndLineId, curIt->ScriptId));
        }

        if (curIt->EndLineId - curIt->StartLineId != rangeLength) {
          throw std::runtime_error(fmt::format(
              "Expected range length {:d} for equivalent line range [{:d}, "
              "{:d}] of script {:d}",
              rangeLength, curIt->StartLineId, curIt->EndLineId,
              curIt->ScriptId));
        }

        // Ranges may not intersect because a single binary search pass is used
        // to identify the equivalent lines
        std::vector<LineRange>& scriptVector =
            ScriptLineRanges.try_emplace(curIt->ScriptId).first->second;
        if (const auto intersected =
                std::ranges::find_if(scriptVector,
                                     [curIt](const auto& other) {
                                       return curIt->Intersects(other);
                                     });
            intersected != scriptVector.end()) {
          throw std::runtime_error(fmt::format(
              "Line range [{:d}, {:d}] of script {:d} intersects with line "
              "range [{:d}, {:d}]",
              curIt->StartLineId, curIt->EndLineId, curIt->ScriptId,
              intersected->StartLineId, intersected->EndLineId));
        }

        curIt->EquivalentLineRangesIdx = EquivalentLineRanges.size() - 1;
        scriptVector.emplace_back(*curIt);
      }
    });

    // Sort the ranges in ScriptLineRanges to be able to binary search
    for (auto& [scriptId, lineRanges] : ScriptLineRanges) {
      std::ranges::sort(lineRanges, std::less<size_t>(), &LineRange::EndLineId);
    }

    Pop();
  }

  Pop();
}

std::vector<std::pair<size_t, size_t>> GetEquivalentLines(const size_t scriptId,
                                                          const size_t lineId) {
  const auto scriptLineRangesIt = ScriptLineRanges.find(scriptId);
  if (scriptLineRangesIt == ScriptLineRanges.end()) return {{scriptId, lineId}};

  const std::vector<LineRange>& scriptLineRanges = scriptLineRangesIt->second;
  const auto lineRange = std::ranges::lower_bound(
      scriptLineRanges, lineId, std::less<size_t>(), &LineRange::EndLineId);
  if (lineRange == scriptLineRanges.end() || lineId < lineRange->StartLineId) {
    return {{scriptId, lineId}};
  }

  const size_t offset = lineId - lineRange->StartLineId;
  std::vector<std::pair<size_t, size_t>> equivalentLines;
  std::ranges::transform(
      EquivalentLineRanges[lineRange->EquivalentLineRangesIdx],
      std::back_inserter(equivalentLines), [offset](const auto& range) {
        return std::pair{range.ScriptId, range.StartLineId + offset};
      });

  return equivalentLines;
}

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto