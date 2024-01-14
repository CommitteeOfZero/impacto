#include "achievementsystem.h"

#include <sstream>
#include "pugixml.hpp"

#include "../../io/physicalfilestream.h"
#include "../../profile/data/achievementsystem.h"
#include "../../log.h"

using namespace Impacto::Profile::AchievementSystem;

namespace Impacto {
namespace CHLCC {

bool AchievementSystem::MountAchievementFile() {
  Io::InputStream* stream;
  IoError err = Io::PhysicalFileStream::Create(AchievementDataPath, &stream);

  if (err != IoError_OK) {
    ImpLog(LL_Fatal, LC_General, "Couldn't open TROPHY.TRP\n");
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
    ImpLog(LL_Fatal, LC_General, "Wrong magic in TROPHY.TRP\n");
    return false;
  }

  // Skipping header padding
  stream->Seek(0x24, SEEK_CUR);

  TrophyDataEntry* tropConfEntry = nullptr;

  for (auto& entry : TrophyDataEntries) {
    Io::ReadArrayBE((int8_t*)entry.name, stream, 32);

    tropConfEntry =
        strncmp(entry.name, "TROPCONF.SFM", 32) == 0 ? &entry : tropConfEntry;

    entry.offset = Io::ReadBE<uint64_t>(stream);
    entry.size = Io::ReadBE<uint64_t>(stream);
    stream->Seek(tdh.entry_size - sizeof(TrophyDataEntry), SEEK_CUR);
  }

  if (tropConfEntry == nullptr) {
    ImpLog(LL_Fatal, LC_General,
           "TROPCONF.SFM entry not found in TROPHY.TRP\n");
    return false;
  }

  stream->Seek(tropConfEntry->offset, SEEK_SET);

  char rawTropConf[tropConfEntry->size + 1];

  Io::ReadArrayBE<int8_t>((int8_t*)rawTropConf, stream, tropConfEntry->size);
  rawTropConf[tropConfEntry->size + 1] = '\0';

  pugi::xml_document tropConf;
  pugi::xml_parse_result result = tropConf.load_string(rawTropConf);

  // TODO
  // if (!result) { ... }

  for (pugi::xml_node trophy = tropConf.first_child().child("trophy"); trophy;
       trophy = trophy.next_sibling()) {
    int id = trophy.attribute("id").as_int();
    Trophies[id].id = id;
    Trophies[id].hidden = trophy.attribute("hidden").as_bool();

    switch (trophy.attribute("ttype").as_string()[0]) {
      case 'P':
        Trophies[id].ttype = Platinum;
        break;
      case 'B':
        Trophies[id].ttype = Bronze;
        break;
      case 'S':
        Trophies[id].ttype = Silver;
        break;
      case 'G':
        Trophies[id].ttype = Gold;
        break;
      default:
        ImpLog(LL_Fatal, LC_General, "Invalid trophy type i TROPCONF.SFM\n");
        return false;
    }
  }

  return true;
}

}  // namespace CHLCC
}  // namespace Impacto