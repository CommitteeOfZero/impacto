#include "achievementsystem.h"

#include <sstream>
#include "pugixml.hpp"
#include "../../io/physicalfilestream.h"
#include "../../profile/data/achievementsystem.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::AchievementSystem;

TrophyDataHeader *WorkingTrophyHeader = nullptr;

AchievementError AchievementSystem::MountAchievementFile() {
  Io::InputStream *stream;
  IoError err = Io::PhysicalFileStream::Create(AchievementDataPath, &stream);

  switch (err) {
    case IoError_NotFound:
      return AchievementNotFound;
  };

  WorkingTrophyHeader = new TrophyDataHeader;

  WorkingTrophyHeader->magic = Io::ReadBE<uint32_t>(stream);
  WorkingTrophyHeader->version = Io::ReadBE<uint32_t>(stream);
  WorkingTrophyHeader->file_size = Io::ReadBE<uint64_t>(stream);
  WorkingTrophyHeader->files_count = Io::ReadBE<uint32_t>(stream);
  WorkingTrophyHeader->element_size = Io::ReadBE<uint32_t>(stream);
  WorkingTrophyHeader->dev_flag = Io::ReadBE<uint32_t>(stream);

  stream->Seek(0x24, SEEK_CUR);

  for (int idx = 0; idx < WorkingTrophyHeader->files_count; idx++) {
    Io::ReadArrayBE<int8_t>((int8_t *)TrophyDataEntries[idx].name, stream, 32);
    TrophyDataEntries[idx].offset = Io::ReadBE<uint64_t>(stream);
    TrophyDataEntries[idx].size = Io::ReadBE<uint64_t>(stream);

    stream->Seek(WorkingTrophyHeader->element_size - 48, SEEK_CUR);
  }

  stream->Seek(TrophyDataEntries[1].offset, SEEK_SET);

  auto raw_xml = (char *)calloc(TrophyDataEntries[1].size, 1);
  Io::ReadArrayBE<int8_t>((int8_t *)raw_xml, stream, TrophyDataEntries[1].size);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(raw_xml);
  pugi::xpath_node_set trophies = doc.select_nodes("//trophy");

  free(raw_xml);

  for (pugi::xpath_node node : trophies) {
    pugi::xml_node trophy = node.node();

    int id = trophy.attribute("id").as_int();
    memset(&Trophies[id], 0, sizeof(Trophies));

    Trophies[id].id = id;
    Trophies[id].hidden =
        strcmp(trophy.attribute("hidden").value(), "yes") == 0;

    if (strcmp(trophy.attribute("ttype").value(), "P") == 0)
      Trophies[id].ttype = Platinum;
    else if (strcmp(trophy.attribute("ttype").value(), "G") == 0)
      Trophies[id].ttype = Gold;
    else if (strcmp(trophy.attribute("ttype").value(), "S") == 0)
      Trophies[id].ttype = Silver;
    else
      Trophies[id].ttype = Bronze;

    strcpy(Trophies[id].name,
           trophy.select_node("name/text()[1]").node().value());
    strcpy(Trophies[id].detail,
           trophy.select_node("detail/text()[1]").node().value());
  }

  return AchievementOK;
}

}  // namespace CHLCC
}  // namespace Impacto