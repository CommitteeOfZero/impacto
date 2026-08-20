#include "achievementsystem.h"

#include <cassert>
#include <cstring>
#include <ctime>

#include "../mem.h"
#include "../profile/data/achievementsystem.h"
#include "../profile/scriptvars.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../util.h"

namespace Impacto {
namespace AchievementSystem {

using namespace Impacto::Profile::AchievementSystem;
using namespace Impacto::Profile::ScriptVars;

static void LoadUnlockState();

void Init() {
  Profile::AchievementSystem::Configure();
  LoadUnlockState();
  if (Implementation) {
    MountAchievementFile();
  }
}

class AchievementFileLoader
    : public Loadable<AchievementFileLoader, AchievementError> {
  friend Loadable<AchievementFileLoader, AchievementError>;

 protected:
  void UnloadSync() {}
  AchievementError LoadSync() {
    if (Implementation) {
      return Implementation->MountAchievementFile(MainThreadCallback);
    }

    return AchievementError::Failed;
  }

  void MainThreadOnLoad(AchievementError result) {
    if (MainThreadCallback) {
      MainThreadCallback();
      MainThreadCallback = nullptr;
    }

    // Let's not report errors until we finalize the implementation
    result = AchievementError::OK;

    ScrWork[SW_SAVEERRORCODE] = (int)result;
  }

 private:
  std::function<void(void)> MainThreadCallback;
};

static AchievementFileLoader Loader;

LoadStatus GetLoadStatus() { return Loader.Status; }

void MountAchievementFile() {
  AchievementError result = Loader.LoadAsync() ? AchievementError::InProgress
                                               : AchievementError::Failed;

  ScrWork[SW_SAVEERRORCODE] = (int)result;
}

const Achievement* GetAchievement(int id) {
  if (!Implementation) return nullptr;
  return Implementation->GetAchievement(id);
}

size_t GetAchievementCount() {
  if (!Implementation) return 0;
  return Implementation->GetAchievementCount();
}

// File layout:
//   Header (16 bytes):
//     0x0-0x3  magic "ACHV"
//     0x4      format version (1)
//     0x5      entry size in bytes (0x10)
//     0x6-0xF  reserved, zero-filled (pads the header to a full hex line)
//   Then one 0x10 (16) byte entry per achievement, indexed by achievement id
//   (each entry occupies exactly one hex line):
//     0x0      unlock state (0 = locked, 1 = unlocked)
//     0x1-0x2  year, uint16 LE (only meaningful if unlocked)
//     0x3      month (1-12)
//     0x4      day (1-31)
//     0x5      hour (0-23)
//     0x6      minute (0-59)
//     0x7      second (0-59)
//     0x8-0xF  reserved, zero-filled
static constexpr char AchievementFileMagic[4] = {'A', 'C', 'H', 'V'};
static constexpr uint8_t AchievementFileVersion = 1;
static constexpr uint64_t AchievementEntrySize = 0x10;
static constexpr uint64_t AchievementHeaderSize = 16;
// magic + version byte + entry size byte
static constexpr uint64_t AchievementHeaderPrefixSize =
    sizeof(AchievementFileMagic) + 2;
static constexpr uint64_t AchievementHeaderReservedSize =
    AchievementHeaderSize - AchievementHeaderPrefixSize;
// unlock byte + year (2) + month + day + hour + minute + second
static constexpr uint64_t AchievementEntryDataSize = 8;
static constexpr uint64_t AchievementEntryReservedSize =
    AchievementEntrySize - AchievementEntryDataSize;
static constexpr uint64_t AchievementDateFieldsSize =
    AchievementEntryDataSize - 1;

struct AchievementUnlockState {
  bool Unlocked = false;
  tm UnlockDate{};
};

static std::vector<AchievementUnlockState> UnlockState;
static bool UnlockStateLoaded = false;

static void LoadUnlockState() {
  UnlockState.clear();
  UnlockStateLoaded = true;
  if (AchievementDataPath.empty()) return;

  Io::Stream* stream;
  if (Io::PhysicalFileStream::Create(AchievementDataPath, &stream) !=
      IoError_OK) {
    return;
  }

  if (static_cast<uint64_t>(stream->Meta.Size) < AchievementHeaderSize) {
    delete stream;
    return;
  }

  char magic[4];
  Io::ReadArrayWithoutSwap<uint8_t>((uint8_t*)magic, stream, sizeof(magic));
  const uint8_t version = Io::ReadU8(stream);
  const uint8_t entrySize = Io::ReadU8(stream);
  if (std::memcmp(magic, AchievementFileMagic, sizeof(magic)) != 0 ||
      static_cast<uint64_t>(entrySize) != AchievementEntrySize) {
    ImpLog(LogLevel::Warning, LogChannel::IO,
           "Achievement unlock data {:s} has an unrecognized format, "
           "ignoring\n",
           AchievementDataPath);
    delete stream;
    return;
  }
  if (version != AchievementFileVersion) {
    ImpLog(LogLevel::Warning, LogChannel::IO,
           "Achievement unlock data {:s} has an incompatible version {:d}, "
           "ignoring\n",
           AchievementDataPath, version);
    delete stream;
    return;
  }
  stream->Seek(AchievementHeaderSize, SEEK_SET);

  const uint64_t count =
      (static_cast<uint64_t>(stream->Meta.Size) - AchievementHeaderSize) /
      AchievementEntrySize;
  UnlockState.resize(count);

  for (uint64_t id = 0; id < count; id++) {
    AchievementUnlockState& state = UnlockState[id];
    state.Unlocked = Io::ReadU8(stream) != 0;
    const uint16_t year = Io::ReadLE<uint16_t>(stream);
    const uint8_t month = Io::ReadU8(stream);
    const uint8_t day = Io::ReadU8(stream);
    const uint8_t hour = Io::ReadU8(stream);
    const uint8_t minute = Io::ReadU8(stream);
    const uint8_t second = Io::ReadU8(stream);
    stream->Seek(AchievementHeaderSize + AchievementEntrySize * (id + 1),
                 SEEK_SET);

    state.UnlockDate = tm{};
    if (state.Unlocked) {
      state.UnlockDate.tm_year = year - 1900;
      state.UnlockDate.tm_mon = month - 1;
      state.UnlockDate.tm_mday = day;
      state.UnlockDate.tm_hour = hour;
      state.UnlockDate.tm_min = minute;
      state.UnlockDate.tm_sec = second;
    }
  }

  delete stream;
}

static void SaveUnlockState() {
  if (AchievementDataPath.empty()) return;

  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      AchievementDataPath, &stream,
      CF::CREATE | CF::CREATE_DIRS | CF::WRITE | CF::TRUNCATE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Couldn't write achievement unlock data {:s}\n",
           AchievementDataPath);
    return;
  }

  Io::WriteArrayWithoutSwap<uint8_t>((uint8_t const*)AchievementFileMagic,
                                     stream, sizeof(AchievementFileMagic));
  Io::WriteU8(stream, AchievementFileVersion);
  Io::WriteU8(stream, (uint8_t)AchievementEntrySize);
  for (uint64_t i = 0; i < AchievementHeaderReservedSize; i++)
    Io::WriteU8(stream, 0);

  for (AchievementUnlockState const& state : UnlockState) {
    Io::WriteU8(stream, state.Unlocked ? 1 : 0);
    if (state.Unlocked) {
      Io::WriteLE<uint16_t>(stream,
                            (uint16_t)(state.UnlockDate.tm_year + 1900));
      Io::WriteU8(stream, (uint8_t)(state.UnlockDate.tm_mon + 1));
      Io::WriteU8(stream, (uint8_t)state.UnlockDate.tm_mday);
      Io::WriteU8(stream, (uint8_t)state.UnlockDate.tm_hour);
      Io::WriteU8(stream, (uint8_t)state.UnlockDate.tm_min);
      Io::WriteU8(stream, (uint8_t)state.UnlockDate.tm_sec);
    } else {
      for (uint64_t i = 0; i < AchievementDateFieldsSize; i++)
        Io::WriteU8(stream, 0);
    }
    for (uint64_t i = 0; i < AchievementEntryReservedSize; i++)
      Io::WriteU8(stream, 0);
  }

  delete stream;
}

bool IsAchievementUnlocked(int id) {
  assert(UnlockStateLoaded);
  if (id < 0 || (size_t)id >= UnlockState.size()) return false;
  return UnlockState[id].Unlocked;
}

tm GetAchievementUnlockDate(int id) {
  assert(UnlockStateLoaded);
  if (id < 0 || (size_t)id >= UnlockState.size()) return tm{};
  return UnlockState[id].UnlockDate;
}

bool UnlockAchievement(int id) {
  assert(UnlockStateLoaded);
  if (id < 0) return false;

  if (id >= std::ssize(UnlockState)) UnlockState.resize(id + 1);
  if (UnlockState[id].Unlocked) return false;

  UnlockState[id].Unlocked = true;
  UnlockState[id].UnlockDate = CurrentDateTime();
  SaveUnlockState();
  return true;
}

}  // namespace AchievementSystem
}  // namespace Impacto