#include "savesystem.h"

#include "../../io/memorystream.h"
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../profile/configsystem.h"
#include "../../data/tipssystem.h"
#include "../../effects/wave.h"
#include "../../audio/audiosystem.h"

#include <cstdint>
#include <numeric>

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Vm;
using namespace Impacto::Effects;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::ConfigSystem;

SaveFileEntry* WorkingSaveEntry = nullptr;

constexpr std::array<float, 4> TextSpeeds = {0x100 / 60.0f, 0x300 / 60.0f,
                                             0x600 / 60.0f, 0xfff0000 / 60.0f};
constexpr uint8_t TextSpeedToSettingIndex(const float speed) {
  for (uint8_t i = 0; i < std::ssize(TextSpeeds) - 1; i++) {
    if (speed <= std::midpoint(TextSpeeds[i], TextSpeeds[i + 1])) return i;
  }

  return std::ssize(TextSpeeds) - 1;
}

constexpr std::array<float, 3> AutoSpeeds = {0x100 / 60.0f, 0x300 / 60.0f,
                                             0x600 / 60.0f};
constexpr uint8_t AutoSpeedToSettingIndex(const float speed) {
  for (uint8_t i = 0; i < std::ssize(AutoSpeeds) - 1; i++) {
    if (speed <= std::midpoint(AutoSpeeds[i], AutoSpeeds[i + 1])) return i;
  }

  return std::ssize(AutoSpeeds) - 1;
}

constexpr uint8_t AutoQuickSaveSettingToIndex(const uint8_t setting) {
  const bool onScene = setting & AutoQuickSaveType::OnScene;
  const bool onTrigger = setting & AutoQuickSaveType::OnTrigger;

  constexpr uint8_t onTriggerIndex = 0;
  constexpr uint8_t onSceneIndex = 1;
  constexpr uint8_t onTriggerAndSceneIndex = 2;
  constexpr uint8_t neverIndex = 3;

  if (onScene && onTrigger) {
    return onTriggerAndSceneIndex;
  } else if (onScene && !onTrigger) {
    return onSceneIndex;
  } else if (!onScene && onTrigger) {
    return onTriggerIndex;
  } else {
    return neverIndex;
  }
}

constexpr uint8_t AutoQuickSaveIndexToSetting(const uint8_t index) {
  switch (index) {
    case 0:
      return AutoQuickSaveType::OnTrigger;
    case 1:
      return AutoQuickSaveType::OnScene;
    case 2:
      return AutoQuickSaveType::OnTrigger | AutoQuickSaveType::OnScene;
    case 3:
      return AutoQuickSaveType::Never;
  }

  ImpLog(LogLevel::Warning, LogChannel::IO,
         "Unexpected auto quick save index {:d}", index);
  return AutoQuickSaveType::OnTrigger | AutoQuickSaveType::OnScene;
}

std::pair<uint8_t, uint8_t> CalculateFileChecksum(
    std::span<const uint8_t> bufferData, uint8_t initSum = 0,
    uint8_t initXor = 0) {
  uint8_t checksumSum = initSum;
  uint8_t checksumXor = initXor;

  for (uint8_t byte : bufferData) {
    checksumSum += byte;
    checksumXor ^= byte;
  }

  return {checksumSum, checksumXor};
}

std::pair<uint8_t, uint8_t> CalculateEntryChecksum(
    std::span<const uint8_t> bufferData, uint16_t initSum = 0,
    uint8_t initXor = 0) {
  uint16_t checksumSum = initSum;
  uint8_t checksumXor = initXor;

  for (uint8_t byte : bufferData) {
    checksumSum += byte;
    checksumXor ^= byte;
  }

  return {(checksumSum >> 4) & 0xFF, checksumXor};
}

std::pair<uint16_t, uint16_t> CalculateSystemChecksum(
    std::span<const uint8_t> bufferData, uint16_t initSum = 0,
    uint16_t initXor = 0) {
  uint32_t checksumSum = initSum;
  uint16_t checksumXor = initXor;

  for (size_t i = 0; i < bufferData.size() - 1; i += 2) {
    const uint16_t dataShort = (bufferData[i] << 8) | bufferData[i + 1];
    checksumSum += dataShort;
    checksumXor ^= dataShort;
  }

  return {checksumSum & 0xFFFF, checksumXor};
}

void SaveSystem::InitializeSystemData() {
  std::ranges::fill(SystemData, 0x00);

  Io::MemoryStream stream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);

  // Config settings
  stream.Seek(0x76c, SEEK_SET);
  Io::WriteLE(&stream, (Uint8)(Default::GroupVolumes[Audio::ACG_Voice] *
                               128));  // VOICE2vol
  Io::WriteLE(&stream, (Uint8)(Default::GroupVolumes[Audio::ACG_Voice] *
                               128));  // VOICEvol
  Io::WriteLE(&stream, (Uint8)(Default::GroupVolumes[Audio::ACG_BGM] * 256));
  Io::WriteLE(&stream,
              (Uint8)(Default::GroupVolumes[Audio::ACG_SE] * 128));  // SEvol
  Io::WriteLE(&stream, (Uint8)(Default::GroupVolumes[Audio::ACG_SE] * 0.75f *
                               128));  // SYSSEvol
  Io::WriteLE(&stream, (Uint8)(Default::GroupVolumes[Audio::ACG_Movie] * 128));
  Io::WriteLE(&stream, TextSpeedToSettingIndex(Default::TextSpeed));
  Io::WriteLE(&stream, AutoSpeedToSettingIndex(Default::AutoSpeed));
  Io::WriteLE(&stream, Default::SyncVoice);
  Io::WriteLE(&stream, !Default::SkipRead);
  Io::WriteLE(&stream, AutoQuickSaveSettingToIndex(Default::AutoQuickSave));

  stream.Seek(0x77c, SEEK_SET);
  Io::WriteLE(&stream, Default::SkipVoice);
  Io::WriteLE(&stream, Default::ShowTipsNotification);
  Io::WriteLE(&stream, Default::ControllerType);
  Io::WriteLE(&stream, Default::TriggerStopSkip);
  Io::WriteLE(&stream, (Uint8)(Default::ImageSize * 128));

  stream.Seek(0x794, SEEK_SET);
  static_assert(Default::VoiceMuted.size() >= 32);
  for (size_t i = 0; i < 32; i++) {
    Io::WriteLE(&stream, !Default::VoiceMuted[i]);
  }

  static_assert(Default::VoiceVolume.size() >= 20);
  for (size_t i = 0; i < 20; i++) {
    Io::WriteLE(&stream, (Uint8)(Default::VoiceVolume[i] * 128));
  }

  std::for_each_n(QuickSaveEntries, MaxSaveEntries,
                  [](auto& ptr) { ptr = new SaveFileEntry(); });
  std::for_each_n(FullSaveEntries, MaxSaveEntries,
                  [](auto& ptr) { ptr = new SaveFileEntry(); });
  WorkingSaveEntry = new SaveFileEntry();

  WorkingSaveThumbnail.Sheet =
      SpriteSheet(static_cast<float>(Window->WindowWidth),
                  static_cast<float>(Window->WindowHeight));
  WorkingSaveThumbnail.Bounds =
      RectF(0.0f, 0.0f, static_cast<float>(Window->WindowWidth),
            static_cast<float>(Window->WindowHeight));

  Texture workingSaveTexture = Texture();
  workingSaveTexture.LoadSolidColor(
      static_cast<int>(WorkingSaveThumbnail.Bounds.Width),
      static_cast<int>(WorkingSaveThumbnail.Bounds.Height), 0x000000);
  WorkingSaveThumbnail.Sheet.Texture = workingSaveTexture.Submit();
}

SaveError SaveSystem::CheckSaveFile() const {
  const static auto checkFile = [](const std::string& filePath, size_t fileSize,
                                   std::string_view logName) {
    std::error_code ec;

    IoError existsState = Io::PathExists(filePath);
    if (existsState == IoError_NotFound) {
      return SaveError::NotFound;
    } else if (existsState == IoError_Fail) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to check if {:s} exists, error: \"{:s}\"\n", logName,
             ec.message());
      return SaveError::Failed;
    }

    auto saveFileSize = Io::GetFileSize(filePath);
    if (saveFileSize == IoError_Fail) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get {:s} size, error: \"{:s}\"\n", logName,
             ec.message());
      return SaveError::Failed;
    } else if (static_cast<size_t>(saveFileSize) != fileSize) {
      return SaveError::Corrupted;
    }

    Io::FilePermissionsFlags perms;
    IoError permsState = Io::GetFilePermissions(filePath, perms);
    using enum Io::FilePermissionsFlags;
    if (permsState == IoError_Fail) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get {:s} permissions, error: \"{:s}\"\n", logName,
             ec.message());
      return SaveError::Failed;
    } else if ((perms & owner_read) == none || (perms & owner_write) == none) {
      return SaveError::WrongUser;
    }

    return SaveError::OK;
  };

  SaveError error = checkFile(SaveFilePath, SaveFileSize, "save file");
  if (error != SaveError::OK) return error;

  error = checkFile(*ThumbnailFilePath, ThumbnailFileSize, "thumbnail file");

  return error;
}

void SaveSystem::SaveSystemData() {
  Io::MemoryStream stream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);

  stream.Seek(0xa, SEEK_SET);

  Io::WriteArrayLE<uint8_t>(&FlagWork[100], &stream, 50);
  Io::WriteArrayLE<uint8_t>(&FlagWork[460], &stream, 40);
  Io::WriteArrayBE<int>(&ScrWork[600], &stream, 400);

  stream.Seek(0x76b, SEEK_SET);
  Io::WriteLE(&stream, QuickSaveCount);

  // Config settings
  stream.Seek(0x76c, SEEK_SET);
  Io::WriteLE(&stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_Voice] *
                               128));  // VOICE2vol
  Io::WriteLE(&stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_Voice] *
                               128));  // VOICEvol
  Io::WriteLE(&stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_BGM] * 256));
  Io::WriteLE(&stream,
              (Uint8)(Audio::GroupVolumes[Audio::ACG_SE] * 128));  // SEvol
  Io::WriteLE(&stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_SE] * 0.75f *
                               128));  // SYSSEvol
  Io::WriteLE(&stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_Movie] * 128));
  Io::WriteLE(&stream, TextSpeedToSettingIndex(TextSpeed));
  Io::WriteLE(&stream, AutoSpeedToSettingIndex(AutoSpeed));
  Io::WriteLE(&stream, SyncVoice);
  Io::WriteLE(&stream, !SkipRead);
  Io::WriteLE(&stream, AutoQuickSaveSettingToIndex(AutoQuickSave));

  stream.Seek(0x77c, SEEK_SET);
  Io::WriteLE(&stream, SkipVoice);
  Io::WriteLE(&stream, ShowTipsNotification);
  Io::WriteLE(&stream, ControllerType);
  Io::WriteLE(&stream, TriggerStopSkip);
  Io::WriteLE(&stream, (Uint8)(ImageSize * 128));

  stream.Seek(0x794, SEEK_SET);
  static_assert(VoiceMuted.size() >= 32);
  for (size_t i = 0; i < 32; i++) {
    Io::WriteLE(&stream, !VoiceMuted[i]);
  }

  static_assert(VoiceVolume.size() >= 20);
  for (size_t i = 0; i < 20; i++) {
    Io::WriteLE(&stream, (Uint8)(VoiceVolume[i] * 128));
  }

  // EV Flags
  stream.Seek(0x7d0, SEEK_SET);
  for (int i = 0; i < 150; i++) {
    uint8_t val = (EVFlags[8 * i] & 1) | ((EVFlags[8 * i + 1] ? 1 : 0) << 1) |
                  ((EVFlags[8 * i + 2] ? 1 : 0) << 2) |
                  ((EVFlags[8 * i + 3] ? 1 : 0) << 3) |
                  ((EVFlags[8 * i + 4] ? 1 : 0) << 4) |
                  ((EVFlags[8 * i + 5] ? 1 : 0) << 5) |
                  ((EVFlags[8 * i + 6] ? 1 : 0) << 6) |
                  (EVFlags[8 * i + 7] << 7);
    Io::WriteU8(&stream, val);
  }

  stream.Seek(0xbb8, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(BGMFlags.data(), &stream, BGMFlags.size());

  stream.Seek(0xc1c, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(MessageFlags.data(), &stream, MessageFlags.size());

  stream.Seek(0x332c, SEEK_SET);
  Io::WriteArrayBE<uint16_t>(TipsSystem::GetNewTipsIndices().data(), &stream,
                             TipsSystem::GetNewTipsIndices().size());
  Io::WriteBE<uint16_t>(&stream, 0,
                        299 - TipsSystem::GetNewTipsIndices().size());
  stream.Seek(0x3582, SEEK_SET);
  Io::WriteLE<uint16_t>(
      &stream, static_cast<uint16_t>(TipsSystem::GetNewTipsIndices().size()));

  stream.Seek(0x3584, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(GameExtraData.data(), &stream,
                            GameExtraData.size());
}

void SaveSystem::LoadEntryBuffer(Io::MemoryStream& memoryStream,
                                 SaveFileEntry& entry) {
  entry.Status = Io::ReadLE<uint8_t>(&memoryStream);
  entry.Checksum = Io::ReadLE<uint16_t>(&memoryStream);
  Io::ReadLE<uint8_t>(&memoryStream);

  const uint8_t saveMonth = Io::ReadLE<uint8_t>(&memoryStream);
  const uint8_t saveDay = Io::ReadLE<uint8_t>(&memoryStream);
  const uint8_t saveHour = Io::ReadLE<uint8_t>(&memoryStream);
  const uint8_t saveMinute = Io::ReadLE<uint8_t>(&memoryStream);
  const uint8_t saveYear = Io::ReadLE<uint8_t>(&memoryStream);
  const uint8_t saveSecond = Io::ReadLE<uint8_t>(&memoryStream);
  entry.SaveDate = tm{};
  entry.SaveDate.tm_sec = saveSecond;
  entry.SaveDate.tm_min = saveMinute;
  entry.SaveDate.tm_hour = saveHour;
  entry.SaveDate.tm_mday = saveDay;
  entry.SaveDate.tm_mon = saveMonth - 1;
  entry.SaveDate.tm_year = saveYear + 2000 - 1900;

  Io::ReadLE<uint16_t>(&memoryStream);
  entry.PlayTime = Io::ReadLE<uint32_t>(&memoryStream);
  entry.SwTitle = Io::ReadLE<uint16_t>(&memoryStream);

  Io::ReadLE<uint8_t>(&memoryStream);
  entry.Flags = Io::ReadLE<uint8_t>(&memoryStream);

  memoryStream.Seek(30, SEEK_CUR);
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript1.data(), &memoryStream,
                           entry.FlagWorkScript1.size());
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript2.data(), &memoryStream,
                           entry.FlagWorkScript2.size());
  Io::ReadArrayBE<int>(entry.ScrWorkScript1.data(), &memoryStream,
                       entry.ScrWorkScript1.size());
  Io::ReadArrayBE<int>(entry.ScrWorkScript2.data(), &memoryStream,
                       entry.ScrWorkScript2.size());

  entry.MainThreadExecPriority = Io::ReadBE<uint32_t>(&memoryStream);
  entry.MainThreadGroupId = Io::ReadBE<uint32_t>(&memoryStream);
  entry.MainThreadWaitCounter = Io::ReadBE<uint32_t>(&memoryStream);
  entry.MainThreadScriptParam = Io::ReadBE<uint32_t>(&memoryStream);
  entry.MainThreadIp = Io::ReadBE<uint32_t>(&memoryStream);
  entry.MainThreadLoopCounter = Io::ReadBE<uint32_t>(&memoryStream);
  entry.MainThreadLoopAdr = Io::ReadBE<uint32_t>(&memoryStream);
  entry.MainThreadCallStackDepth = Io::ReadBE<uint32_t>(&memoryStream);

  for (int j = 0; j < 8; j++) {
    entry.MainThreadReturnAddresses[j] = Io::ReadBE<uint32_t>(&memoryStream);
    entry.MainThreadReturnBufIds[j] = Io::ReadBE<uint32_t>(&memoryStream);
  }

  Io::ReadArrayBE<int>(entry.MainThreadVariables.data(), &memoryStream, 16);
  entry.MainThreadDialoguePageId = Io::ReadBE<uint32_t>(&memoryStream);

  Io::ReadArrayLE<uint16_t>(entry.WaveData.data(), &memoryStream,
                            entry.WaveData.size());
  memoryStream.Seek(1224, SEEK_CUR);
}

void SaveSystem::SaveEntryBuffer(Io::MemoryStream& memoryStream,
                                 SaveFileEntry& entry) {
  Io::WriteLE<uint8_t>(&memoryStream, entry.Status);
  Io::WriteLE<uint16_t>(&memoryStream, (uint16_t)entry.Checksum);

  Io::WriteLE<uint8_t>(&memoryStream, 0);
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)(entry.SaveDate.tm_mon + 1));
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_mday);
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_hour);
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_min);
  Io::WriteLE<uint8_t>(&memoryStream,
                       (uint8_t)(entry.SaveDate.tm_year + 1900 - 2000));
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_sec);

  Io::WriteLE<uint16_t>(&memoryStream, 0);
  Io::WriteLE<uint32_t>(&memoryStream, entry.PlayTime);
  Io::WriteLE<uint16_t>(&memoryStream, (uint16_t)entry.SwTitle);

  Io::WriteLE<uint8_t>(&memoryStream, 0);
  Io::WriteLE<uint8_t>(&memoryStream, entry.Flags);

  memoryStream.Seek(30, SEEK_CUR);
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript1.data(), &memoryStream,
                            entry.FlagWorkScript1.size());
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript2.data(), &memoryStream,
                            entry.FlagWorkScript2.size());
  Io::WriteArrayBE<int>(entry.ScrWorkScript1.data(), &memoryStream,
                        entry.ScrWorkScript1.size());
  Io::WriteArrayBE<int>(entry.ScrWorkScript2.data(), &memoryStream,
                        entry.ScrWorkScript2.size());

  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadExecPriority);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadGroupId);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadWaitCounter);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadScriptParam);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadIp);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadLoopCounter);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadLoopAdr);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadCallStackDepth);

  for (int j = 0; j < 8; j++) {
    Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadReturnAddresses[j]);
    Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadReturnBufIds[j]);
  }

  Io::WriteArrayBE<int>(entry.MainThreadVariables.data(), &memoryStream, 16);
  Io::WriteBE<uint32_t>(&memoryStream, entry.MainThreadDialoguePageId);

  Io::WriteArrayLE<uint16_t>(entry.WaveData.data(), &memoryStream,
                             entry.WaveData.size());
  memoryStream.Seek(1224, SEEK_CUR);
}

SaveError SaveSystem::LoadSystemData() {
  Io::MemoryStream stream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);

  stream.Seek(0xa, SEEK_SET);

  Io::ReadArrayLE<uint8_t>(&FlagWork[100], &stream, 50);
  Io::ReadArrayLE<uint8_t>(&FlagWork[460], &stream, 40);
  Io::ReadArrayBE<int>(&ScrWork[600], &stream, 400);

  stream.Seek(0x76b, SEEK_SET);
  QuickSaveCount = Io::ReadLE<Uint8>(&stream);
  // Config settings
  stream.Seek(0x76c, SEEK_SET);
  stream.Seek(1, SEEK_CUR);  // VOICE2vol
  Audio::GroupVolumes[Audio::ACG_Voice] = Io::ReadLE<Uint8>(&stream) / 128.0f;
  Audio::GroupVolumes[Audio::ACG_BGM] = Io::ReadLE<Uint8>(&stream) / 256.0f;
  Audio::GroupVolumes[Audio::ACG_SE] = Io::ReadLE<Uint8>(&stream) / 128.0f;
  stream.Seek(1, SEEK_CUR);  // SYSSEvol
  Audio::GroupVolumes[Audio::ACG_Movie] = Io::ReadLE<Uint8>(&stream) / 128.0f;
  TextSpeed = TextSpeeds[Io::ReadLE<Uint8>(&stream)];
  AutoSpeed = AutoSpeeds[Io::ReadLE<Uint8>(&stream)];
  SyncVoice = Io::ReadLE<bool>(&stream);
  SkipRead = !Io::ReadLE<bool>(&stream);
  AutoQuickSave = AutoQuickSaveIndexToSetting(Io::ReadLE<Uint8>(&stream));

  stream.Seek(0x77c, SEEK_SET);
  SkipVoice = Io::ReadLE<bool>(&stream);
  ShowTipsNotification = Io::ReadLE<bool>(&stream);
  ControllerType = Io::ReadLE<Uint8>(&stream);
  TriggerStopSkip = Io::ReadLE<bool>(&stream);
  ImageSize = Io::ReadLE<Uint8>(&stream) / 128.0f;

  stream.Seek(0x794, SEEK_SET);
  static_assert(VoiceMuted.size() >= 32);
  for (size_t i = 0; i < 32; i++) {
    VoiceMuted[i] = !Io::ReadLE<bool>(&stream);
  }

  static_assert(VoiceVolume.size() >= 20);
  for (size_t i = 0; i < 20; i++) {
    VoiceVolume[i] = Io::ReadLE<Uint8>(&stream) / 128.0f;
  }

  // EV Flags
  stream.Seek(0x7d0, SEEK_SET);
  for (int i = 0; i < 150; i++) {
    auto val = Io::ReadU8(&stream);
    EVFlags[8 * i] = val & 1;
    EVFlags[8 * i + 1] = (val & 2) != 0;
    EVFlags[8 * i + 2] = (val & 4) != 0;
    EVFlags[8 * i + 3] = (val & 8) != 0;
    EVFlags[8 * i + 4] = (val & 0x10) != 0;
    EVFlags[8 * i + 5] = (val & 0x20) != 0;
    EVFlags[8 * i + 6] = (val & 0x40) != 0;
    EVFlags[8 * i + 7] = val >> 7;
  }

  stream.Seek(0xbb8, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(BGMFlags.data(), &stream, BGMFlags.size());

  stream.Seek(0xc1c, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(MessageFlags.data(), &stream, MessageFlags.size());

  stream.Seek(0x3582, SEEK_SET);
  uint16_t newTipsCount = Io::ReadLE<uint16_t>(&stream);
  TipsSystem::GetNewTipsIndices() = std::vector<uint16_t>(newTipsCount);
  stream.Seek(0x332c, SEEK_SET);
  Io::ReadArrayBE<uint16_t>(TipsSystem::GetNewTipsIndices().data(), &stream,
                            newTipsCount);

  stream.Seek(0x3584, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(GameExtraData.data(), &stream, GameExtraData.size());

  return SaveError::OK;
}

SaveError SaveSystem::MountSaveFile(std::vector<QueuedTexture>& textures) {
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(SaveFilePath, &stream);
  switch (err) {
    case IoError_NotFound:
      return SaveError::NotFound;
    case IoError_Fail:
    case IoError_Eof:
      return SaveError::Corrupted;
    case IoError_OK:
      break;
  };

  WorkingSaveEntry = new SaveFileEntry();
  WorkingSaveThumbnail.Sheet =
      SpriteSheet((float)Window->WindowWidth, (float)Window->WindowHeight);
  WorkingSaveThumbnail.Bounds = RectF(0.0f, 0.0f, (float)Window->WindowWidth,
                                      (float)Window->WindowHeight);

  QueuedTexture txt{
      .Id = std::ref(WorkingSaveThumbnail.Sheet.Texture),
  };
  txt.Tex.LoadSolidColor((int)WorkingSaveThumbnail.Bounds.Width,
                         (int)WorkingSaveThumbnail.Bounds.Height, 0x000000);
  textures.push_back(txt);

  stream->Seek(0x0, SEEK_SET);
  const uint8_t readFileSystemChecksumSum = Io::ReadU8(stream);
  const uint8_t readFileSystemChecksumXor = Io::ReadU8(stream);
  const uint8_t readFileEntriesChecksumSum = Io::ReadU8(stream);
  const uint8_t readFileEntriesChecksumXor = Io::ReadU8(stream);
  const uint8_t readFileThumbnailsChecksumSum = Io::ReadU8(stream);
  const uint8_t readFileThumbnailsChecksumXor = Io::ReadU8(stream);

  stream->Seek(0xa, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(SystemData.data(), stream, SystemData.size());

  const auto [calcFileSystemChecksumSum, calcFileSystemChecksumXor] =
      CalculateFileChecksum(SystemData);
  if (readFileSystemChecksumSum != calcFileSystemChecksumSum ||
      readFileSystemChecksumXor != calcFileSystemChecksumXor) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Save file system data checksum doesn't match\n");
  }

  const uint16_t readSystemChecksumSum = (SystemData[1] << 8) | SystemData[0];
  const uint16_t readSystemChecksumXor = (SystemData[3] << 8) | SystemData[2];
  const auto [calcSystemChecksumSum, calcSystemChecksumXor] =
      CalculateSystemChecksum(
          std::span(SystemData).subspan(10, 0x1cbd * sizeof(uint16_t)));

  if (readSystemChecksumSum != calcSystemChecksumSum ||
      readSystemChecksumXor != calcSystemChecksumXor) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "System data checksum doesn't match\n");
  }

  uint8_t calcFileEntriesChecksumSum = 0;
  uint8_t calcFileEntriesChecksumXor = 0;
  int lockedQuickSaveSlots = 0;
  for (auto& entryArray : {QuickSaveEntries, FullSaveEntries}) {
    SaveType saveType =
        (entryArray == QuickSaveEntries) ? SaveType::Quick : SaveType::Full;
    for (int i = 0; i < MaxSaveEntries; i++) {
      entryArray[i] = new SaveFileEntry();

      std::array<uint8_t, SaveEntrySize> entrySlotBuf;
      Io::ReadArrayLE<uint8_t>(entrySlotBuf.data(), stream,
                               entrySlotBuf.size());

      std::tie(calcFileEntriesChecksumSum, calcFileEntriesChecksumXor) =
          CalculateFileChecksum(entrySlotBuf, calcFileEntriesChecksumSum,
                                calcFileEntriesChecksumXor);

      const uint8_t readEntryChecksumSum = entrySlotBuf[1];
      const uint8_t readEntryChecksumXor = entrySlotBuf[2];
      if (readEntryChecksumSum != 0 || readEntryChecksumXor != 0) {
        const auto [calcEntryChecksumSum, calcEntryChecksumXor] =
            CalculateEntryChecksum(std::span(entrySlotBuf).subspan(4), 0x76,
                                   0x12);

        const bool entryChecksumOk =
            readEntryChecksumSum == calcEntryChecksumSum &&
            readEntryChecksumXor == calcEntryChecksumXor;
        entrySlotBuf[0] = entryChecksumOk ? 1 : 2;
      }

      Io::MemoryStream saveEntryDataStream(entrySlotBuf.data(),
                                           entrySlotBuf.size(), false);

      LoadEntryBuffer(saveEntryDataStream,
                      static_cast<SaveFileEntry&>(*entryArray[i]));
      if (saveType == SaveType::Quick) {
        lockedQuickSaveSlots +=
            static_cast<SaveFileEntry&>(*entryArray[i]).Flags & WriteProtect;
      }
    }
  }

  SetLockedQuickSaveCount(lockedQuickSaveSlots);

  // CHLCC doesn't use a separate recents list
  std::ranges::sort(QuickSaveRecentSortedId,
                    Impacto::SaveSystem::SaveRecencyComparator(),
                    [this](int id) { return *QuickSaveEntries[id]; });

  if (readFileEntriesChecksumSum != calcFileEntriesChecksumSum ||
      readFileEntriesChecksumXor != calcFileEntriesChecksumXor) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Save file entries checksum doesn't match\n");
  }

  delete stream;

  // Load thumbnails

  err = Io::PhysicalFileStream::Create(*ThumbnailFilePath, &stream);
  switch (err) {
    case IoError_NotFound:
      return SaveError::NotFound;
    case IoError_Fail:
    case IoError_Eof:
      return SaveError::Corrupted;
    case IoError_OK:
      break;
  };

  constexpr size_t thumbnailPaddingSize =
      SaveThumbnailWidth * SaveThumbnailHeight;

  uint8_t calcFileThumbnailsChecksumSum = 0;
  uint8_t calcFileThumbnailsChecksumXor = 0;

  for (auto& entryArray : {QuickSaveEntries, FullSaveEntries}) {
    for (int i = 0; i < MaxSaveEntries; i++) {
      SaveFileEntry* const entry = static_cast<SaveFileEntry*>(entryArray[i]);

      if (entry->Status == 0) {
        stream->Seek(SaveThumbnailSize + thumbnailPaddingSize, SEEK_CUR);
        continue;
      }

      QueuedTexture texture{
          .Id = std::ref(entry->SaveThumbnail.Sheet.Texture),
      };
      texture.Tex.Init(TexFmt_RGB, SaveThumbnailWidth, SaveThumbnailHeight);

      Sprite& thumbnail = entry->SaveThumbnail;
      thumbnail.Sheet = SpriteSheet(SaveThumbnailWidth, SaveThumbnailHeight);
      thumbnail.Bounds =
          RectF(0.0f, 0.0f, SaveThumbnailWidth, SaveThumbnailHeight);

      Io::ReadArrayLE<uint8_t>(entry->ThumbnailData.data(), stream,
                               entry->ThumbnailData.size());
      stream->Seek(thumbnailPaddingSize, SEEK_CUR);

      texture.Tex.Buffer.assign(entry->ThumbnailData.begin(),
                                entry->ThumbnailData.end());
      textures.push_back(texture);

      std::tie(calcFileThumbnailsChecksumSum, calcFileThumbnailsChecksumXor) =
          CalculateFileChecksum(entry->ThumbnailData,
                                calcFileThumbnailsChecksumSum,
                                calcFileThumbnailsChecksumXor);
    }
  }

  delete stream;

  if (readFileThumbnailsChecksumSum != calcFileThumbnailsChecksumSum ||
      readFileThumbnailsChecksumXor != calcFileThumbnailsChecksumXor) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Save file thumbnails checksum doesn't match\n");
  }

  return SaveError::OK;
}

void SaveSystem::FlushWorkingSaveEntry(SaveType type, int id,
                                       int autoSaveType) {
  auto* entry = GetSaveEntry<SaveFileEntry>(type, id);
  if (WorkingSaveEntry != nullptr && entry != nullptr &&
      !(entry->Flags & WriteProtect)) {
    if (type == SaveType::Quick) {
      entry->SaveType = autoSaveType;
      UpdateQuickSaveRecentSortedId(id);
      if (QuickSaveCount != MaxSaveEntries) {
        QuickSaveCount++;
      }
    }
    uint8_t savedFlags = entry->Flags;
    *entry = *WorkingSaveEntry;
    entry->Flags = savedFlags;

    entry->SaveDate = CurrentDateTime();

    std::vector<uint8_t> captureBuffer =
        Renderer->GetSpriteSheetImage(WorkingSaveThumbnail.Sheet);
    Texture tex;
    tex.Init(TexFmt_RGBA, SaveThumbnailWidth, SaveThumbnailHeight);

    entry->SaveThumbnail.Sheet =
        SpriteSheet(SaveThumbnailWidth, SaveThumbnailHeight);
    entry->SaveThumbnail.Bounds =
        RectF(0.0f, 0.0f, SaveThumbnailWidth, SaveThumbnailHeight);

    if (ResizeImage(WorkingSaveThumbnail.Bounds, entry->SaveThumbnail.Bounds,
                    captureBuffer, tex.Buffer) < 0) {
      ImpLog(LogLevel::Error, LogChannel::General,
             "Failed to resize save thumbnail\n");
    }
    entry->SaveThumbnail.Sheet.Texture = tex.Submit();
  }
}

SaveError SaveSystem::WriteSaveFile() {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      SaveFilePath, &stream, CF::CREATE | CF::CREATE_DIRS | CF::WRITE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return SaveError::Failed;
  }

  const auto [systemChecksumSum, systemChecksumXor] = CalculateSystemChecksum(
      std::span(SystemData).subspan(10, 0x1cbd * sizeof(uint16_t)));
  SystemData[0] = systemChecksumSum & 0xFF;
  SystemData[1] = systemChecksumSum >> 8;
  SystemData[2] = systemChecksumXor & 0xFF;
  SystemData[3] = systemChecksumXor >> 8;

  const auto [fileSystemChecksumSum, fileSystemChecksumXor] =
      CalculateFileChecksum(SystemData);

  stream->Seek(0x0, SEEK_SET);
  Io::WriteU8(stream, fileSystemChecksumSum);
  Io::WriteU8(stream, fileSystemChecksumXor);

  stream->Seek(0xa, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(SystemData.data(), stream, SystemData.size());

  uint8_t fileEntriesChecksumSum = 0;
  uint8_t fileEntriesChecksumXor = 0;

  [[maybe_unused]] int64_t saveDataPos = stream->Position;
  auto writeEntry = [&](SaveFileEntry* entry, int i) {
    assert(stream->Position - saveDataPos == (int64_t)(i * SaveEntrySize));

    if (entry == nullptr || entry->Status == 0) {
      Io::WriteLE<uint8_t>(stream, 0, SaveEntrySize);
    } else {
      std::array<uint8_t, SaveEntrySize> entrySlotBuf{};
      Io::MemoryStream saveEntryMemoryStream(entrySlotBuf.data(),
                                             entrySlotBuf.size(), false);
      SaveEntryBuffer(saveEntryMemoryStream, *entry);

      const auto [entryChecksumSum, entryChecksumXor] = CalculateEntryChecksum(
          std::span(entrySlotBuf).subspan(4), 0x76, 0x12);
      entrySlotBuf[0] = 1;
      entrySlotBuf[1] = entryChecksumSum;
      entrySlotBuf[2] = entryChecksumXor;

      std::tie(fileEntriesChecksumSum, fileEntriesChecksumXor) =
          CalculateFileChecksum(entrySlotBuf, fileEntriesChecksumSum,
                                fileEntriesChecksumXor);

      Io::WriteArrayLE<uint8_t>(entrySlotBuf.data(), stream,
                                entrySlotBuf.size());
    }
  };

  // Chlcc stores quick saves from oldest to newest, with no separate recency
  // array.
  for (int i = 0; i < MaxSaveEntries; ++i) {
    int reverseI = MaxSaveEntries - i - 1;
    auto* entry = GetSaveEntry<SaveFileEntry>(SaveType::Quick, reverseI);
    writeEntry(entry, i);
  }
  saveDataPos = stream->Position;
  for (int i = 0; i < MaxSaveEntries; ++i) {
    auto* entry = GetSaveEntry<SaveFileEntry>(SaveType::Full, i);
    writeEntry(entry, i);
  }

  stream->Seek(0x2, SEEK_SET);
  Io::WriteU8(stream, fileEntriesChecksumSum);
  Io::WriteU8(stream, fileEntriesChecksumXor);

  // Write thumbnails

  Io::Stream* thumbnailsStream;
  err =
      Io::PhysicalFileStream::Create(*ThumbnailFilePath, &thumbnailsStream,
                                     CF::CREATE | CF::CREATE_DIRS | CF::WRITE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open thumbnail file for writing\n");
    return SaveError::Failed;
  }

  uint8_t fileThumbnailsChecksumSum = 0;
  uint8_t fileThumbnailsChecksumXor = 0;

  for (auto& entryArray : {QuickSaveEntries, FullSaveEntries}) {
    for (size_t i = 0; i < MaxSaveEntries; i++) {
      SaveFileEntry* entry = static_cast<SaveFileEntry*>(entryArray[i]);

      if (entry == nullptr || entry->Status == 0) {
        Io::WriteLE<uint8_t>(thumbnailsStream, 0,
                             SaveThumbnailSize + ThumbnailPaddingSize);
      } else {
        Io::WriteArrayLE<uint8_t>(entry->ThumbnailData.data(), thumbnailsStream,
                                  entry->ThumbnailData.size());
        Io::WriteLE<uint8_t>(thumbnailsStream, 0, ThumbnailPaddingSize);

        std::tie(fileThumbnailsChecksumSum, fileThumbnailsChecksumXor) =
            CalculateFileChecksum(entry->ThumbnailData,
                                  fileThumbnailsChecksumSum,
                                  fileThumbnailsChecksumXor);
      }
    }
  }

  stream->Seek(0x4, SEEK_SET);
  Io::WriteU8(stream, fileThumbnailsChecksumSum);
  Io::WriteU8(stream, fileThumbnailsChecksumXor);

  // Four empty bytes between the end of checksums and the start of SystemData
  Io::WriteLE<uint8_t>(stream, 0x00, 4);

  delete stream;
  delete thumbnailsStream;

  return SaveError::OK;
}

uint32_t SaveSystem::GetSavePlayTime(SaveType type, int id) const {
  return GetSaveEntry<SaveFileEntry>(type, id)->PlayTime;
}

uint8_t SaveSystem::GetSaveFlags(SaveType type, int id) const {
  return GetSaveEntry<SaveFileEntry>(type, id)->Flags;
}

void SaveSystem::SetSaveFlags(SaveType type, int id, uint8_t flags) {
  auto* entry = GetSaveEntry<SaveFileEntry>(type, id);
  if (type == SaveType::Quick) {
    uint8_t currentFlags = entry->Flags;
    if ((currentFlags ^ flags) & WriteProtect) {
      if (flags & WriteProtect) {
        LockedQuickSaveCount++;
      } else {
        LockedQuickSaveCount--;
      }

      SetFlag(SF_SAVEALLPROTECTED, LockedQuickSaveCount == MaxSaveEntries);
    }
  }
  entry->Flags = flags;
}

tm const& SaveSystem::GetSaveDate(SaveType type, int id) const {
  return GetSaveEntry<SaveFileEntry>(type, id)->SaveDate;
}

void SaveSystem::SaveMemory() {
  if (WorkingSaveEntry == nullptr) return;

  WorkingSaveEntry->Status = 1;

  const tm timeInfo = CurrentDateTime();
  WorkingSaveEntry->SaveDate = timeInfo;
  WorkingSaveEntry->PlayTime = ScrWork[SW_PLAYTIME];
  WorkingSaveEntry->SwTitle = ScrWork[SW_TITLE];

  std::copy(FlagWork.begin() + 50,
            FlagWork.begin() + 50 + WorkingSaveEntry->FlagWorkScript1.size(),
            WorkingSaveEntry->FlagWorkScript1.begin());
  std::copy(FlagWork.begin() + 300,
            FlagWork.begin() + 300 + WorkingSaveEntry->FlagWorkScript2.size(),
            WorkingSaveEntry->FlagWorkScript2.begin());
  std::copy(ScrWork.begin() + 300,
            ScrWork.begin() + 300 + WorkingSaveEntry->ScrWorkScript1.size(),
            WorkingSaveEntry->ScrWorkScript1.begin());
  std::copy(ScrWork.begin() + 2300,
            ScrWork.begin() + 2300 + WorkingSaveEntry->ScrWorkScript2.size(),
            WorkingSaveEntry->ScrWorkScript2.begin());

  int threadId = ScrWork[SW_MAINTHDP];
  Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];
  if (thd != nullptr && 4 <= thd->GroupId && thd->GroupId <= 6) {
    WorkingSaveEntry->MainThreadExecPriority = thd->ExecPriority;
    WorkingSaveEntry->MainThreadWaitCounter = thd->WaitCounter;
    WorkingSaveEntry->MainThreadScriptParam = thd->ScriptParam;
    WorkingSaveEntry->MainThreadGroupId = thd->GroupId << 16;
    WorkingSaveEntry->MainThreadGroupId |= thd->ScriptBufferId;
    WorkingSaveEntry->MainThreadIp = thd->IpOffset;
    WorkingSaveEntry->MainThreadCallStackDepth = thd->CallStackDepth;

    for (uint32_t i = 0; i < thd->CallStackDepth; i++) {
      WorkingSaveEntry->MainThreadReturnAddresses[i] = thd->ReturnAddresses[i];
      WorkingSaveEntry->MainThreadReturnBufIds[i] =
          thd->ReturnScriptBufferIds[i];
    }

    memcpy(WorkingSaveEntry->MainThreadVariables.data(), thd->Variables, 64);
    WorkingSaveEntry->MainThreadDialoguePageId = thd->DialoguePageId;
  }
  WaveSave(std::span(WorkingSaveEntry->WaveData));
}

void SaveSystem::LoadEntry(SaveType type, int id) {
  const auto* entry = GetSaveEntry<SaveFileEntry>(type, id);

  if (entry->Status) {
    ScrWork[SW_PLAYTIME] = entry->PlayTime;
    ScrWork[SW_TITLE] = entry->SwTitle;

    std::ranges::copy(entry->FlagWorkScript1, FlagWork.begin() + 50);
    std::ranges::copy(entry->FlagWorkScript2, FlagWork.begin() + 300);
    std::ranges::copy(entry->ScrWorkScript1, ScrWork.begin() + 300);
    std::ranges::copy(entry->ScrWorkScript2, ScrWork.begin() + 2300);

    // TODO: What to do about this mess I wonder...
    ScrWork[SW_SVSENO] = ScrWork[SW_SEREQNO];
    ScrWork[SW_SVSENO + 1] = ScrWork[SW_SEREQNO + 1];
    ScrWork[SW_SVSENO + 2] = ScrWork[SW_SEREQNO + 2];
    ScrWork[SW_SVBGMNO] = ScrWork[SW_BGMREQNO];
    ScrWork[SW_SVSCRNO1] = ScrWork[SW_SCRIPTNO2];
    ScrWork[SW_SVSCRNO2] = ScrWork[SW_SCRIPTNO3];
    ScrWork[SW_SVSCRNO3] = ScrWork[SW_SCRIPTNO4];
    ScrWork[SW_SVSCRNO4] = ScrWork[SW_SCRIPTNO5];
    ScrWork[SW_SVBGNO1] = ScrWork[SW_BG1NO];
    ScrWork[SW_SVBGNO1 + 1] = ScrWork[2427];
    ScrWork[SW_SVBGNO1 + 2] = ScrWork[2447];
    ScrWork[SW_SVBGNO1 + 3] = ScrWork[2467];
    ScrWork[SW_SVBGNO1 + 4] = ScrWork[2487];
    ScrWork[SW_SVBGNO1 + 5] = ScrWork[2507];
    ScrWork[SW_SVBGNO1 + 6] = ScrWork[2527];
    ScrWork[SW_SVBGNO1 + 7] = ScrWork[2547];
    ScrWork[SW_SVCHANO1] = ScrWork[SW_CHA1NO];
    ScrWork[SW_SVCHANO1 + 1] = ScrWork[2629];
    ScrWork[SW_SVCHANO1 + 2] = ScrWork[2649];
    ScrWork[SW_SVCHANO1 + 3] = ScrWork[2669];
    ScrWork[SW_SVCHANO1 + 4] = ScrWork[2689];
    ScrWork[SW_SVCHANO1 + 5] = ScrWork[2709];
    ScrWork[SW_SVCHANO1 + 6] = ScrWork[2729];
    ScrWork[SW_SVCHANO1 + 7] = ScrWork[2749];
    ScrWork[SW_SVCHANO1 + 8] = ScrWork[2769];
    ScrWork[SW_SVCHANO1 + 9] = ScrWork[2789];
    ScrWork[SW_SVCHANO1 + 10] = ScrWork[2809];
    ScrWork[SW_SVCHANO1 + 11] = ScrWork[2829];
    ScrWork[SW_SVCHANO1 + 12] = ScrWork[2849];
    ScrWork[SW_SVCHANO1 + 13] = ScrWork[2869];
    ScrWork[SW_SVCHANO1 + 14] = ScrWork[2889];
    ScrWork[SW_SVCHANO1 + 15] = ScrWork[2909];
    ScrWork[2034] = ScrWork[3200];
    ScrWork[2035] = ScrWork[3201];
    ScrWork[2036] = ScrWork[3202];
    ScrWork[2037] = ScrWork[3203];
    ScrWork[2038] = ScrWork[3204];
    ScrWork[2039] = ScrWork[3205];
    ScrWork[2040] = ScrWork[3206];
    ScrWork[2041] = ScrWork[3207];
    ScrWork[2042] = ScrWork[3208];
    ScrWork[2043] = ScrWork[3209];
    ScrWork[2044] = ScrWork[3210];
    ScrWork[2045] = ScrWork[3211];
    ScrWork[2046] = ScrWork[3212];
    ScrWork[2047] = ScrWork[3213];
    ScrWork[2048] = ScrWork[3214];
    ScrWork[2049] = ScrWork[3215];
    ScrWork[2050] = ScrWork[3216];
    ScrWork[2051] = ScrWork[3220];
    ScrWork[2052] = ScrWork[3221];
    ScrWork[2053] = ScrWork[3222];
    ScrWork[2054] = ScrWork[3223];
    ScrWork[2055] = ScrWork[3224];
    ScrWork[2056] = ScrWork[3225];
    ScrWork[2057] = ScrWork[3226];
    ScrWork[2058] = ScrWork[3227];
    ScrWork[2059] = ScrWork[3228];
    ScrWork[2060] = ScrWork[3229];
    ScrWork[2061] = ScrWork[3230];
    ScrWork[2062] = ScrWork[3231];
    ScrWork[2063] = ScrWork[3232];
    ScrWork[2064] = ScrWork[3233];
    ScrWork[2065] = ScrWork[3234];
    ScrWork[2066] = ScrWork[3235];
    ScrWork[2067] = ScrWork[3236];

    int threadId = ScrWork[SW_MAINTHDP];
    Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];
    if (thd != nullptr &&
        (thd->GroupId == 4 || thd->GroupId == 5 || thd->GroupId == 6)) {
      thd->ExecPriority = entry->MainThreadExecPriority;
      thd->WaitCounter = entry->MainThreadWaitCounter;
      thd->ScriptParam = entry->MainThreadScriptParam;
      thd->GroupId = entry->MainThreadGroupId >> 16;
      thd->ScriptBufferId = entry->MainThreadGroupId & 0xFFFF;
      thd->IpOffset = entry->MainThreadIp;
      thd->CallStackDepth = entry->MainThreadCallStackDepth;

      for (size_t i = 0; i < thd->CallStackDepth; i++) {
        thd->ReturnScriptBufferIds[i] = entry->MainThreadReturnBufIds[i];
        thd->ReturnAddresses[i] = entry->MainThreadReturnAddresses[i];
      }

      memcpy(thd->Variables, entry->MainThreadVariables.data(),
             16 * sizeof(int));
      thd->DialoguePageId = entry->MainThreadDialoguePageId;
    }
    WaveLoad(std::span(entry->WaveData));
  }
}

uint8_t SaveSystem::GetSaveStatus(SaveType type, int id) const {
  return GetSaveEntry<SaveFileEntry>(type, id)->Status;
}

int SaveSystem::GetSaveTitle(SaveType type, int id) const {
  return GetSaveEntry<SaveFileEntry>(type, id)->SwTitle;
}

uint32_t SaveSystem::GetTipStatus(size_t tipId) const {
  tipId *= 3;
  uint8_t lockStatus = (GameExtraData[tipId >> 3] & Flbit[tipId & 7]) != 0;
  uint8_t newStatus =
      (GameExtraData[(tipId + 1) >> 3] & Flbit[(tipId + 1) & 7]) != 0;
  uint8_t unreadStatus =
      (GameExtraData[(tipId + 2) >> 3] & Flbit[(tipId + 2) & 7]) != 0;
  return (lockStatus | (unreadStatus << 1)) | (newStatus << 2);
}

void SaveSystem::SetTipStatus(size_t tipId, bool isLocked, bool isUnread,
                              bool isNew) {
  tipId *= 3;
  if (isLocked) {
    GameExtraData[tipId >> 3] &= ~(Flbit[tipId & 7]);
  } else {
    GameExtraData[tipId >> 3] |= Flbit[tipId & 7];
  }
  if (isUnread) {
    GameExtraData[(tipId + 2) >> 3] &= ~(Flbit[(tipId + 2) & 7]);
  } else {
    GameExtraData[(tipId + 2) >> 3] |= Flbit[(tipId + 2) & 7];
  }
  if (isNew) {
    GameExtraData[(tipId + 1) >> 3] &= ~(Flbit[(tipId + 1) & 7]);
  } else {
    GameExtraData[(tipId + 1) >> 3] |= Flbit[(tipId + 1) & 7];
  }
}

void SaveSystem::SetLineRead(int scriptId, int lineId) {
  if (std::ssize(ScriptMessageData) <= scriptId) return;

  uint32_t offset = ScriptMessageData[scriptId].SaveDataOffset + lineId;
  if (offset == 0xFFFFFFFF) return;

  // TODO: update some ScrWorks (2003, 2005 & 2006)

  MessageFlags[offset >> 3] |= Flbit[offset & 0b111];
}

bool SaveSystem::IsLineRead(int scriptId, int lineId) const {
  if (std::ssize(ScriptMessageData) <= scriptId) return false;

  uint32_t offset = ScriptMessageData[scriptId].SaveDataOffset + lineId;
  uint8_t flbit = Flbit[offset & 0b111];
  uint8_t viewed = MessageFlags[offset >> 3];

  return (bool)(flbit & viewed);
}

void SaveSystem::GetReadMessagesCount(int* totalMessageCount,
                                      int* readMessageCount) const {
  *totalMessageCount = 0;
  *readMessageCount = 0;

  for (int scriptId = 0; scriptId < StoryScriptCount; scriptId++) {
    ScriptMessageDataPair script = ScriptMessageData[scriptId];
    *totalMessageCount += script.LineCount;

    for (size_t lineId = 0; lineId < script.LineCount; lineId++) {
      *readMessageCount += IsLineRead(scriptId, (int)lineId);
    }
  }
}

void SaveSystem::GetViewedEVsCount(int* totalEVCount,
                                   int* viewedEVCount) const {
  for (int i = 0; i < MaxAlbumEntries; i++) {
    if (AlbumEvData[i][0] == 0xFFFF) break;
    for (int j = 0; j < MaxAlbumSubEntries; j++) {
      if (AlbumEvData[i][j] == 0xFFFF) break;
      *totalEVCount += 1;
      *viewedEVCount += EVFlags[AlbumEvData[i][j]];
    }
  }
}
void SaveSystem::GetEVStatus(int evId, int* totalVariations,
                             int* viewedVariations) const {
  *totalVariations = 0;
  *viewedVariations = 0;
  for (int i = 0; i < MaxAlbumSubEntries; i++) {
    if (AlbumEvData[evId][i] == 0xFFFF) break;
    *totalVariations += 1;
    *viewedVariations += EVFlags[AlbumEvData[evId][i]];
  }
}

void SaveSystem::SetEVStatus(int id) { EVFlags[id] = true; }

bool SaveSystem::GetEVVariationIsUnlocked(size_t evId,
                                          size_t variationIdx) const {
  if (AlbumEvData[evId][variationIdx] == 0xFFFF) return false;
  return EVFlags[AlbumEvData[evId][variationIdx]];
}

bool SaveSystem::GetBgmFlag(int id) const { return BGMFlags[id]; }
void SaveSystem::SetBgmFlag(int id, bool flag) { BGMFlags[id] = flag; }

void SaveSystem::SaveThumbnailData() {
  // Renderer expects RGB32
  std::vector<uint8_t> thumbnailBuffer(SaveThumbnailWidth *
                                       SaveThumbnailHeight * 4);

  for (auto* entryArray : {FullSaveEntries, QuickSaveEntries}) {
    for (int i = 0; i < MaxSaveEntries; i++) {
      SaveFileEntry* const entry = static_cast<SaveFileEntry*>(entryArray[i]);
      if (entry->Status == 0) continue;

      Renderer->GetSpriteSheetImage(entry->SaveThumbnail.Sheet,
                                    thumbnailBuffer);

      std::array<uint8_t, SaveThumbnailSize>& thumbnailData =
          entry->ThumbnailData;

      for (size_t pixelId = 0; pixelId < thumbnailBuffer.size() / 4;
           pixelId++) {
        thumbnailData[pixelId * 3 + 0] = thumbnailBuffer[pixelId * 4 + 0];  // r
        thumbnailData[pixelId * 3 + 1] = thumbnailBuffer[pixelId * 4 + 1];  // g
        thumbnailData[pixelId * 3 + 2] = thumbnailBuffer[pixelId * 4 + 2];  // b
      }
    }
  }
}

Sprite& SaveSystem::GetSaveThumbnail(SaveType type, int id) {
  return GetSaveEntry<SaveFileEntry>(type, id)->SaveThumbnail;
}

void SaveSystem::WaveSave(std::span<uint16_t> data) {
  size_t dataOff = 0;
  for (size_t i = 0; i < 10; i++) {
    data[dataOff++] = static_cast<uint16_t>(WaveBG.WaveData[i].Flags);
    data[dataOff++] = static_cast<uint16_t>(WaveEFF.WaveData[i].Flags);

    data[dataOff++] = static_cast<uint16_t>(WaveBG.WaveData[i].Amplitude);
    data[dataOff++] = static_cast<uint16_t>(WaveEFF.WaveData[i].Amplitude);

    data[dataOff++] =
        static_cast<uint16_t>(WaveBG.WaveData[i].TemporalFrequency);
    data[dataOff++] =
        static_cast<uint16_t>(WaveEFF.WaveData[i].TemporalFrequency);

    data[dataOff++] = static_cast<uint16_t>(WaveBG.WaveData[i].Phase);
    data[dataOff++] = static_cast<uint16_t>(WaveEFF.WaveData[i].Phase);

    data[dataOff++] =
        static_cast<uint16_t>(WaveBG.WaveData[i].SpatialFrequency);
    data[dataOff++] =
        static_cast<uint16_t>(WaveEFF.WaveData[i].SpatialFrequency);
  }

  data[dataOff++] = static_cast<uint16_t>(WaveBG.WaveCount);
  data[dataOff++] = static_cast<uint16_t>(WaveEFF.WaveCount);
}

void SaveSystem::WaveLoad(std::span<const uint16_t> data) const {
  size_t dataOff = 0;
  for (size_t i = 0; i < 10; i++) {
    WaveBG.WaveData[i].Flags = static_cast<int>(data[dataOff++]);
    WaveEFF.WaveData[i].Flags = static_cast<int>(data[dataOff++]);

    WaveBG.WaveData[i].Amplitude = static_cast<int>(data[dataOff++]);
    WaveEFF.WaveData[i].Amplitude = static_cast<int>(data[dataOff++]);

    WaveBG.WaveData[i].TemporalFrequency = static_cast<int>(data[dataOff++]);
    WaveEFF.WaveData[i].TemporalFrequency = static_cast<int>(data[dataOff++]);

    WaveBG.WaveData[i].Phase = static_cast<int>(data[dataOff++]);
    WaveEFF.WaveData[i].Phase = static_cast<int>(data[dataOff++]);

    WaveBG.WaveData[i].SpatialFrequency = static_cast<int>(data[dataOff++]);
    WaveEFF.WaveData[i].SpatialFrequency = static_cast<int>(data[dataOff++]);
  }

  WaveBG.WaveCount = static_cast<uint32_t>(data[dataOff++]);
  WaveEFF.WaveCount = static_cast<uint32_t>(data[dataOff++]);
}

}  // namespace CHLCC
}  // namespace Impacto