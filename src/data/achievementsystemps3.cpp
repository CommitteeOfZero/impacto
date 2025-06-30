#include "achievementsystemps3.h"

#include <sstream>
#include <vector>
#include "pugixml.hpp"

#include "../io/physicalfilestream.h"
#include "../io/uncompressedstream.h"
#include "../profile/data/achievementsystem.h"
#include "../log.h"
#include "../texture/texture.h"

int constexpr ICON_START = 3;
int constexpr ICON_SIZE = 240;

using namespace Impacto::Profile::AchievementSystem;

namespace Impacto {
namespace AchievementSystem {

size_t AchievementSystemPS3::GetAchievementCount() const {
  return Trophies.size();
}

AchievementError AchievementSystemPS3::MountAchievementFile() {
  Io::Stream* baseStream;
  IoError err =
      Io::PhysicalFileStream::Create(AchievementDataPath, &baseStream);

  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO, "Couldn't open TROPHY.TRP\n");
    return AchievementError::Failed;
  }

  TrophyDataHeader tdh = {0};
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

  for (pugi::xml_node trophy = trop.first_child().child("trophy"); trophy;
       trophy = trophy.next_sibling()) {
    int id = trophy.attribute("id").as_int();
    bool hidden = trophy.attribute("hidden").as_bool();
    std::string name = trophy.child("name").text().as_string();
    std::string description = trophy.child("detail").text().as_string();
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

    auto& entry = TrophyDataEntries[id + ICON_START];

    Io::Stream* iconStream;
    err = Io::UncompressedStream::Create(baseStream, entry.offset, entry.size,
                                         &iconStream);

    if (err != IoError_OK) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Couldn't open icon for TROP{:03d}\n", id);
      delete baseStream;
      return AchievementError::Failed;
    }

    Texture texture;
    texture.Init(TexFmt_RGBA, ICON_SIZE, ICON_SIZE);

    if (!texture.Load(iconStream)) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Unable to load texture for TROP{:03d}.PNG\n", id);
      delete iconStream;
      delete baseStream;
      return AchievementError::Failed;
    }
    delete iconStream;

    SpriteSheet sheet;
    sheet.DesignWidth = sheet.DesignHeight = ICON_SIZE;
    sheet.Texture = texture.Submit();

    Sprite icon = Sprite(sheet, 0, 0, ICON_SIZE, ICON_SIZE);
    // Ensure the vector is large enough
    if (Trophies.size() <= id) {
      Trophies.resize(id + 1);
    }
    Trophies[id] =
        std::make_unique<Trophy>(std::move(name), std::move(description),
                                 hidden, (TrophyType)ttype, icon);
  }

  delete baseStream;
  return AchievementError::OK;
}

const Achievement* AchievementSystemPS3::GetAchievement(int id) {
  if (id < 0 || id >= Trophies.size()) return nullptr;
  return Trophies[id].get();
}
}  // namespace AchievementSystem
}  // namespace Impacto