#include "achievementsystemps3.h"

#include <sstream>
#include <vector>
#include "pugixml.hpp"

#include "../io/physicalfilestream.h"
#include "../io/uncompressedstream.h"
#include "../profile/data/achievementsystem.h"
#include "../log.h"

int constexpr ICON_START = 3;
int constexpr ICON_SIZE = 240;

using namespace Impacto::Profile::AchievementSystem;

namespace Impacto {
namespace AchievementSystem {

size_t AchievementSystemPS3::GetAchievementCount() const {
  return Trophies.size();
}

AchievementError AchievementSystemPS3::MountAchievementFile(
    std::function<void(void)>& mainThreadCallback) {
  Io::Stream* baseStream;
  IoError err =
      Io::PhysicalFileStream::Create(AchievementDataPath, &baseStream);

  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO, "Couldn't open TROPHY.TRP\n");
    return AchievementError::Failed;
  }

  TrophyDataHeader tdh{};
  tdh.magic = Io::ReadBE<uint32_t>(baseStream);
  tdh.version = Io::ReadBE<uint32_t>(baseStream);
  tdh.file_size = Io::ReadBE<uint64_t>(baseStream);
  tdh.file_count = Io::ReadBE<uint32_t>(baseStream);
  tdh.entry_size = Io::ReadBE<uint32_t>(baseStream);
  tdh.dev_flag = Io::ReadBE<uint32_t>(baseStream);

  if (tdh.magic != PS3_MAGIC) {
    ImpLog(LogLevel::Error, LogChannel::IO, "Wrong magic in TROPHY.TRP\n");
    delete baseStream;
    return AchievementError::Failed;
  }
  TrophyDataEntries.resize(tdh.file_count);

  // Skipping header padding
  baseStream->Seek(0x24, RW_SEEK_CUR);

  TrophyDataEntry* tropEntry = nullptr;

  for (auto& entry : TrophyDataEntries) {
    Io::ReadArrayWithoutSwap<32>((int8_t*)entry.name, baseStream);

    tropEntry = strncmp(entry.name, "TROP.SFM", 32) == 0 ? &entry : tropEntry;

    entry.offset = Io::ReadBE<uint64_t>(baseStream);
    entry.size = Io::ReadBE<uint64_t>(baseStream);
    baseStream->Seek(tdh.entry_size - sizeof(TrophyDataEntry), SEEK_CUR);
  }

  if (tropEntry == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "TROP.SFM entry not found in TROPHY.TRP\n");
    delete baseStream;
    return AchievementError::Failed;
  }

  baseStream->Seek(tropEntry->offset, RW_SEEK_SET);

  // std::string is immutable and Io::Read uses memcpy
  // Variable-length arrays aren't supported in MSVC
  std::vector<char> rawTrop(tropEntry->size + 1, 0);

  Io::ReadArrayBE<int8_t>((int8_t*)rawTrop.data(), baseStream, tropEntry->size);

  pugi::xml_document trop;
  pugi::xml_parse_result result = trop.load_string(rawTrop.data());

  if (!result) {
    ImpLog(LogLevel::Error, LogChannel::IO, "Unable to load TROP.SFM\n");
    delete baseStream;
    return AchievementError::Failed;
  }

  std::vector<QueuedTrophy> queuedTrophies;

  for (pugi::xml_node trophy = trop.first_child().child("trophy"); trophy;
       trophy = trophy.next_sibling()) {
    QueuedTrophy queuedTrophy;
    queuedTrophy.id = trophy.attribute("id").as_int();
    queuedTrophy.hidden = trophy.attribute("hidden").as_bool();
    queuedTrophy.name = trophy.child("name").text().as_string();
    queuedTrophy.description = trophy.child("detail").text().as_string();
    const char* ttypeStr = trophy.attribute("ttype").as_string();

    if (strlen(ttypeStr) == 0) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Missing trophy type in TROP.SFM\n");
      delete baseStream;
      return AchievementError::Failed;
    }

    const char ttype = ttypeStr[0];
    if (strchr(TROPHY_TYPES, ttype) == nullptr) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Invalid trophy type in TROP.SFM\n");
      delete baseStream;
      return AchievementError::Failed;
    }

    queuedTrophy.ttype = (TrophyType)ttype;

    auto& entry = TrophyDataEntries[queuedTrophy.id + ICON_START];

    Io::Stream* iconStream;
    err = Io::UncompressedStream::Create(baseStream, entry.offset, entry.size,
                                         &iconStream);

    if (err != IoError_OK) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Couldn't open icon for TROP{:03d}\n", queuedTrophy.id);
      delete baseStream;
      return AchievementError::Failed;
    }

    queuedTrophy.texture.Init(TexFmt_RGBA, ICON_SIZE, ICON_SIZE);

    if (!queuedTrophy.texture.Load(iconStream)) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Unable to load texture for TROP{:03d}.PNG\n", queuedTrophy.id);
      delete iconStream;
      delete baseStream;
      return AchievementError::Failed;
    }
    delete iconStream;

    queuedTrophies.push_back(std::move(queuedTrophy));
  }

  delete baseStream;

  mainThreadCallback = [this,
                        queuedTrophies = std::move(queuedTrophies)]() mutable {
    for (QueuedTrophy& trophy : queuedTrophies) {
      SpriteSheet sheet;
      sheet.DesignWidth = sheet.DesignHeight = ICON_SIZE;
      sheet.Texture = trophy.texture.Submit();

      Sprite icon = Sprite(sheet, 0, 0, ICON_SIZE, ICON_SIZE);
      // Ensure the vector is large enough
      if ((int)Trophies.size() <= trophy.id) {
        Trophies.resize(trophy.id + 1);
      }
      Trophies[trophy.id] = std::make_unique<Trophy>(
          std::move(trophy.name), std::move(trophy.description), trophy.hidden,
          trophy.ttype, icon);
    }
  };

  return AchievementError::OK;
}

const Achievement* AchievementSystemPS3::GetAchievement(int id) {
  if (id < 0 || (size_t)id >= Trophies.size()) return nullptr;
  return Trophies[id].get();
}
}  // namespace AchievementSystem
}  // namespace Impacto