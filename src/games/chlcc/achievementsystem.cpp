#include "achievementsystem.h"

#include <sstream>
#include <vector>
#include "pugixml.hpp"

#include "../../io/physicalfilestream.h"
#include "../../profile/data/achievementsystem.h"
#include "../../log.h"
#include "../../texture/texture.h"

#define ICON_START 3
#define ICON_SIZE 240

using namespace Impacto::Profile::AchievementSystem;

namespace Impacto {
namespace CHLCC {

bool AchievementSystem::MountAchievementFile() {
  Io::InputStream* stream;
  IoError err = Io::PhysicalFileStream::Create(AchievementDataPath, &stream);

  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Couldn't open TROPHY.TRP\n");
    delete stream;
    return false;
  }

  TrophyDataHeader tdh = {0};
  tdh.magic = Io::ReadBE<uint32_t>(stream);
  tdh.version = Io::ReadBE<uint32_t>(stream);
  tdh.file_size = Io::ReadBE<uint64_t>(stream);
  tdh.file_count = Io::ReadBE<uint32_t>(stream);
  tdh.entry_size = Io::ReadBE<uint32_t>(stream);
  tdh.dev_flag = Io::ReadBE<uint32_t>(stream);

  if (tdh.magic != PS3_MAGIC) {
    ImpLog(LL_Error, LC_IO, "Wrong magic in TROPHY.TRP\n");
    delete stream;
    return false;
  }

  // Skipping header padding
  stream->Seek(0x24, SEEK_CUR);

  TrophyDataEntry* tropEntry = nullptr;

  for (auto& entry : TrophyDataEntries) {
    Io::ReadArrayBE((int8_t*)entry.name, stream, 32);

    tropEntry = strncmp(entry.name, "TROP.SFM", 32) == 0 ? &entry : tropEntry;

    entry.offset = Io::ReadBE<uint64_t>(stream);
    entry.size = Io::ReadBE<uint64_t>(stream);
    stream->Seek(tdh.entry_size - sizeof(TrophyDataEntry), SEEK_CUR);
  }

  if (tropEntry == nullptr) {
    ImpLog(LL_Error, LC_IO, "TROP.SFM entry not found in TROPHY.TRP\n");
    delete stream;
    return false;
  }

  stream->Seek(tropEntry->offset, SEEK_SET);

  // std::string is immutable and Io::Read uses memcpy
  // Variable-length arrays aren't supported in MSVC
  std::vector<char> rawTrop(tropEntry->size + 1, 0);

  Io::ReadArrayBE<int8_t>((int8_t*)rawTrop.data(), stream, tropEntry->size);

  pugi::xml_document trop;
  pugi::xml_parse_result result = trop.load_string(rawTrop.data());

  if (!result) {
    ImpLog(LL_Error, LC_IO, "Unable to load TROP.SFM\n");
    delete stream;
    return false;
  }

  for (pugi::xml_node trophy = trop.first_child().child("trophy"); trophy;
       trophy = trophy.next_sibling()) {
    int id = trophy.attribute("id").as_int();
    bool hidden = trophy.attribute("hidden").as_bool();
    std::string name = trophy.child("name").text().as_string();
    std::string description = trophy.child("detail").text().as_string();

    TrophyType ttype;

    switch (trophy.attribute("ttype").as_string()[0]) {
      case 'P':
        ttype = Platinum;
        break;
      case 'B':
        ttype = Bronze;
        break;
      case 'S':
        ttype = Silver;
        break;
      case 'G':
        ttype = Gold;
        break;
      default:
        ImpLog(LL_Error, LC_IO, "Invalid trophy type in TROP.SFM\n");
        delete stream;
        return false;
    }

    stream->Seek(TrophyDataEntries[id + ICON_START].offset, SEEK_SET);

    Texture texture;
    texture.Init(TexFmt_RGBA, ICON_SIZE, ICON_SIZE);

    if (!texture.Load(stream)) {
      ImpLog(LL_Error, LC_IO, "Unable to load TROP%03d.PNG\n");
      delete stream;
      return false;
    }

    SpriteSheet sheet;
    sheet.DesignWidth = sheet.DesignHeight = ICON_SIZE;
    sheet.Texture = texture.Submit();

    Sprite icon = Sprite(sheet, 0, 0, ICON_SIZE, ICON_SIZE);

    Trophies[id] = new Trophy(std::move(name), std::move(description), hidden,
                              ttype, std::move(icon));
  }

  delete stream;
  return true;
}

}  // namespace CHLCC
}  // namespace Impacto