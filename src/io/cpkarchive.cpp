#include "cpkarchive.h"

#include "../log.h"
#include "../util.h"
#include "uncompressedstream.h"
#include <SDL_endian.h>

namespace Impacto {
namespace Io {

struct CpkColumn {
  enum Kind : uint8_t {
    U8 = 0,
    I8,
    U16,
    I16,
    U32,
    I32,
    U64,
    I64,
    Float,
    Double,
    String,
    Data
  };

  enum Storage : uint8_t {
    NONE = 0,
    DEFAULT = 1,
    CONSTANT = 3,
    NORMAL = 5,
  };
  uint32_t Flags;
  Kind GetType() const { return static_cast<Kind>(Flags & 0xF); }
  Storage GetStorage() const {
    return static_cast<Storage>((Flags >> 4) & 0xF);
  }

  std::string Name;
  std::vector<CpkCell> Cells;
  CpkCell Constant;
};

struct CpkMetaEntry : public FileMeta {
  int64_t Offset;
  int64_t CompressedSize;
  bool Compressed;
};

CpkArchive::~CpkArchive() {
  if (FileList) delete[] FileList;
}

void DecryptUtfBlock(uint8_t* utfBlock, uint64_t size) {
  uint32_t utfDecryptVal = 0x655f;
  for (uint64_t i = 0; i < size; i++) {
    utfBlock[i] ^= utfDecryptVal & 0xFF;
    utfDecryptVal *= 0x4115;
  }
}

bool CpkArchive::ReadUtfBlock(
    std::vector<uint8_t>& utfBlock,
    std::vector<ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                             std::equal_to<>>>& rows) {
  const uint32_t utfMagic = 0x40555446;
  UtfStream = new MemoryStream(utfBlock.data(), utfBlock.size(), false);
  if (ReadBE<uint32_t>(UtfStream) != utfMagic) {
    DecryptUtfBlock(utfBlock.data(), utfBlock.size());
    UtfStream->Seek(0, RW_SEEK_SET);
    if (ReadBE<uint32_t>(UtfStream) != utfMagic) {
      ImpLog(LogLevel::Trace, LogChannel::IO, "Error reading CPK UTF table\n");
      delete UtfStream;
      return false;
    }
  }

  // uint32_t tableSize = ReadBE<uint32_t>(UtfStream);
  UtfStream->Seek(4, RW_SEEK_CUR);
  int64_t rowsOffset = ReadBE<uint32_t>(UtfStream);
  int64_t stringsOffset = ReadBE<uint32_t>(UtfStream);
  int64_t dataOffset = ReadBE<uint32_t>(UtfStream);

  rowsOffset += 8;
  stringsOffset += 8;
  dataOffset += 8;

  // uint32_t tableNameOffset = ReadBE<uint32_t>(UtfStream);
  UtfStream->Seek(4, RW_SEEK_CUR);
  uint16_t numColumns = ReadBE<uint16_t>(UtfStream);
  uint16_t rowLength = ReadBE<uint16_t>(UtfStream);
  uint32_t numRows = ReadBE<uint32_t>(UtfStream);

  std::vector<CpkColumn> columns;

  const auto readCell = [this, &stringsOffset,
                         &dataOffset](CpkColumn const& column) {
    const auto readOrDefaultT = []<typename T>(CpkColumn::Storage storeType,
                                               Stream* s) {
      if (storeType == CpkColumn::Storage::DEFAULT) return T{};
      return Io::ReadBE<T>(s);
    };

    CpkCell cell{};
    auto storeType = column.GetStorage();
    switch (column.GetType()) {
      case CpkColumn::Kind::U8:
        cell = readOrDefaultT.operator()<uint8_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::I8:
        cell = readOrDefaultT.operator()<int8_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::U16:
        cell = readOrDefaultT.operator()<uint16_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::I16:
        cell = readOrDefaultT.operator()<int16_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::U32:
        cell = readOrDefaultT.operator()<uint32_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::I32:
        cell = readOrDefaultT.operator()<int32_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::U64:
        cell = readOrDefaultT.operator()<uint64_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::I64:
        cell = readOrDefaultT.operator()<int64_t>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::Float:
        cell = readOrDefaultT.operator()<float>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::Double:
        cell = readOrDefaultT.operator()<double>(storeType, UtfStream);
        break;
      case CpkColumn::Kind::String:
        if (storeType == CpkColumn::Storage::DEFAULT) {
          cell = std::string();
        } else {
          cell = ReadString(stringsOffset);
        }
        break;
      case CpkColumn::Kind::Data: {
        if (storeType == CpkColumn::Storage::DEFAULT) {
          cell = std::vector<uint8_t>();
        } else {
          int64_t dataPos = ReadBE<uint32_t>(UtfStream) + dataOffset;
          uint64_t dataSize = ReadBE<uint32_t>(UtfStream);
          uint64_t retAddr = UtfStream->Position;
          std::vector<uint8_t> dataBuf(dataSize + sizeof(uint64_t));
          UtfStream->Seek(dataPos, RW_SEEK_SET);
          UtfStream->Read(dataBuf.data(), dataSize);
          UtfStream->Seek(retAddr, RW_SEEK_SET);
          cell = std::move(dataBuf);
        }
      } break;
    }
    return cell;
  };

  for (int i = 0; i < numColumns; i++) {
    CpkColumn column;
    column.Flags = ReadU8(UtfStream);
    if (column.Flags == 0) column.Flags = ReadBE<uint32_t>(UtfStream);

    column.Name = ReadString(stringsOffset);
    if (column.GetStorage() == CpkColumn::Storage::CONSTANT) {
      column.Constant = readCell(column);
    }

    columns.push_back(column);
  }

  for (uint32_t i = 0; i < numRows; i++) {
    UtfStream->Seek(rowsOffset + (i * rowLength), RW_SEEK_SET);
    ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                 std::equal_to<>>
        row;
    for (auto& column : columns) {
      row[column.Name] = (column.GetStorage() == CpkColumn::Storage::CONSTANT)
                             ? column.Constant
                             : readCell(column);
    }
    rows.push_back(row);
  }

  delete UtfStream;
  return true;
}

std::string CpkArchive::ReadString(int64_t stringsOffset) {
  int64_t stringAddr = stringsOffset + ReadBE<uint32_t>(UtfStream);

  int64_t retAddr = UtfStream->Position;

  UtfStream->Seek(stringAddr, RW_SEEK_SET);

  char ch;
  std::string output;
  while ((ch = ReadU8(UtfStream)) != 0x00) {
    output.push_back(ch);
  }
  UtfStream->Seek(retAddr, RW_SEEK_SET);
  return output;
}

CpkMetaEntry* CpkArchive::GetFileListEntry(uint32_t id) {
  CpkMetaEntry* entry;
  auto it = IdsToFiles.find(id);
  if (it == IdsToFiles.end()) {
    entry = &FileList[NextFile];
    IdsToFiles[id] = &FileList[NextFile];
    NextFile++;
  } else {
    entry = (CpkMetaEntry*)it->second;
  }
  return entry;
}

IoError CpkArchive::ReadItoc(int64_t itocOffset, int64_t contentOffset,
                             uint16_t align) {
  BaseStream->Seek(itocOffset, RW_SEEK_SET);
  const uint32_t itocMagic = 0x49544F43;
  if (ReadBE<uint32_t>(BaseStream) != itocMagic) {
    ImpLog(LogLevel::Trace, LogChannel::IO, "Error reading CPK ITOC\n");
    return IoError_Fail;
  }
  BaseStream->Seek(4, RW_SEEK_CUR);
  uint64_t utfSize = ReadLE<uint64_t>(BaseStream);
  std::vector<uint8_t> utfBlock(utfSize);
  BaseStream->Read(utfBlock.data(), utfSize);
  std::vector<ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                           std::equal_to<>>>
      itocUtfTable;
  if (!ReadUtfBlock(utfBlock, itocUtfTable)) return IoError_Fail;
  auto lowDataItr = itocUtfTable[0].find("DataL");
  if (lowDataItr != itocUtfTable[0].end()) {
    std::vector<ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                             std::equal_to<>>>
        dataLUtfTable;
    if (!ReadUtfBlock(std::get<std::vector<uint8_t>>(itocUtfTable[0]["DataL"]),
                      dataLUtfTable))
      return IoError_Fail;
    std::vector<ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                             std::equal_to<>>>
        dataHUtfTable;
    if (!ReadUtfBlock(std::get<std::vector<uint8_t>>(itocUtfTable[0]["DataH"]),
                      dataHUtfTable))
      return IoError_Fail;

    for (auto& row : dataLUtfTable) {
      int id = std::get<uint16_t>(row["ID"]);
      CpkMetaEntry* entry = GetFileListEntry(id);
      entry->Id = id;

      uint16_t extractedSize = std::get<uint16_t>(row["ExtractSize"]);
      uint16_t fileSize = std::get<uint16_t>(row["FileSize"]);
      if (extractedSize && (extractedSize != fileSize)) {
        entry->Size = extractedSize;
        entry->CompressedSize = fileSize;
        entry->Compressed = true;
      } else {
        entry->Size = fileSize;
        entry->CompressedSize = fileSize;
        entry->Compressed = false;
      }
      if (entry->FileName.empty()) {
        char path[6];
        snprintf(path, 6, "%05i", id);
        entry->FileName = path;
      }
    }

    for (auto& row : dataHUtfTable) {
      int id = std::get<uint16_t>(row["ID"]);
      CpkMetaEntry* entry = GetFileListEntry(id);
      entry->Id = id;

      int extractedSize = std::get<uint32_t>(row["ExtractSize"]);
      int fileSize = std::get<uint32_t>(row["FileSize"]);
      if (extractedSize && (extractedSize != fileSize)) {
        entry->Size = extractedSize;
        entry->CompressedSize = fileSize;
        entry->Compressed = true;
      } else {
        entry->Size = fileSize;
        entry->CompressedSize = fileSize;
        entry->Compressed = false;
      }
      if (entry->FileName.empty()) {
        char path[6];
        snprintf(path, 6, "%05i", id);
        entry->FileName = path;
      }
    }
  }

  int64_t offset = contentOffset;
  std::vector<std::pair<uint32_t, FileMeta*>> fileVec(IdsToFiles.begin(),
                                                      IdsToFiles.end());
  std::sort(fileVec.begin(), fileVec.end());
  for (const auto& kv : fileVec) {
    int64_t size;
    CpkMetaEntry* entry = (CpkMetaEntry*)kv.second;
    if (entry->CompressedSize > 0) {
      size = entry->CompressedSize;
    } else {
      size = entry->Size;
    }
    entry->Offset = offset;
    offset += size;
    if (size % align) offset += align - (size % align);
  }

  return IoError_OK;
}

IoError CpkArchive::ReadToc(int64_t tocOffset, int64_t contentOffset) {
  BaseStream->Seek(tocOffset, RW_SEEK_SET);
  const uint32_t tocMagic = 0x544F4320;
  if (ReadBE<uint32_t>(BaseStream) != tocMagic) {
    ImpLog(LogLevel::Trace, LogChannel::IO, "Error reading CPK TOC\n");
    return IoError_Fail;
  }
  BaseStream->Seek(4, RW_SEEK_CUR);
  uint64_t utfSize = ReadLE<uint64_t>(BaseStream);
  std::vector<uint8_t> utfBlock(utfSize);
  BaseStream->Read(utfBlock.data(), utfSize);
  std::vector<ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                           std::equal_to<>>>
      tocUtfTable;
  if (!ReadUtfBlock(utfBlock, tocUtfTable)) return IoError_Fail;

  for (auto& row : tocUtfTable) {
    uint32_t id = std::get<uint32_t>(row["ID"]);
    CpkMetaEntry* entry = GetFileListEntry(id);

    entry->Id = id;
    entry->Offset = std::get<uint64_t>(row["FileOffset"]);
    constexpr static int CpkMaxPath = 244;
    char path[CpkMaxPath * 2] = {0};

    if (auto* dir = std::get_if<std::string>(&row["DirName"]);
        dir && !dir->empty()) {
      fmt::format_to_n(path, CpkMaxPath * 2, "{}/{}", *dir,
                       std::get<std::string>(row["FileName"]));
    } else {
      fmt::format_to_n(path, CpkMaxPath, "{}",
                       std::get<std::string>(row["FileName"]));
    }
    if (strlen(path) == 0) {
      snprintf(path, CpkMaxPath, "%05i", id);
    }
    entry->FileName = path;

    int extractedSize = std::get<uint32_t>(row["ExtractSize"]);
    int fileSize = std::get<uint32_t>(row["FileSize"]);
    if (extractedSize && (extractedSize != fileSize)) {
      entry->Size = extractedSize;
      entry->CompressedSize = fileSize;
      entry->Compressed = true;
    } else {
      entry->Size = fileSize;
      entry->CompressedSize = fileSize;
      entry->Compressed = false;
    }
  }

  return IoError_OK;
}

IoError CpkArchive::ReadEtoc(int64_t etocOffset) {
  BaseStream->Seek(etocOffset, RW_SEEK_SET);
  const uint32_t etocMagic = 0x45544F43;
  if (ReadBE<uint32_t>(BaseStream) != etocMagic) {
    ImpLog(LogLevel::Trace, LogChannel::IO, "Error reading CPK ETOC\n");
    return IoError_Fail;
  }
  BaseStream->Seek(4, RW_SEEK_CUR);
  uint64_t utfSize = ReadLE<uint64_t>(BaseStream);
  std::vector<uint8_t> utfBlock(utfSize);
  BaseStream->Read(utfBlock.data(), utfSize);
  std::vector<ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                           std::equal_to<>>>
      etocUtfTable;
  if (!ReadUtfBlock(utfBlock, etocUtfTable)) return IoError_Fail;

  // for (auto& row : etocUtfTable) {
  // TODO: This contains the LocalDir and UpdateDateTime params. Do we actually
  // need this?...
  //}

  return IoError_OK;
}

IoError CpkArchive::Create(Stream* stream, VfsArchive** outArchive) {
  ImpLog(LogLevel::Trace, LogChannel::IO, "Trying to mount \"{:s}\" as CPK\n",
         stream->Meta.FileName);

  CpkArchive* result = 0;

  std::vector<ankerl::unordered_dense::map<std::string, CpkCell, string_hash,
                                           std::equal_to<>>>
      headerUtfTable;

  uint16_t alignVal;

  auto errorHandler = [&] {
    stream->Seek(0, RW_SEEK_SET);
    if (result) delete result;
    return IoError_Fail;
  };

  uint32_t const magic = 0x43504B20;
  if (ReadBE<uint32_t>(stream) != magic) {
    ImpLog(LogLevel::Trace, LogChannel::IO, "Not a CPK\n");
    return errorHandler();
  }

  result = new CpkArchive;
  result->BaseStream = stream;

  bool encrypted = ReadLE<uint32_t>(stream) == 0;
  {
    uint64_t utfSize = ReadLE<uint64_t>(stream);
    std::vector<uint8_t> utfBlock(utfSize);
    if (encrypted) {
      uint8_t key = 0x5F;
      for (auto& elem : utfBlock) {
        elem = elem ^ key;
        key = (key * 0x15) & 0xFF;
      }
    }
    stream->Read(utfBlock.data(), utfSize);
    if (!result->ReadUtfBlock(utfBlock, headerUtfTable)) {
      return errorHandler();
    }
  }

  result->FileCount = std::get<uint32_t>(headerUtfTable[0]["Files"]);
  result->Version = std::get<uint16_t>(headerUtfTable[0]["Version"]);
  result->Revision = std::get<uint16_t>(headerUtfTable[0]["Revision"]);

  alignVal = std::get<uint16_t>(headerUtfTable[0]["Align"]);

  result->FileList = new CpkMetaEntry[result->FileCount];

  if (std::get<uint64_t>(headerUtfTable[0]["TocOffset"]) != 0) {
    result->ReadToc(std::get<uint64_t>(headerUtfTable[0]["TocOffset"]),
                    std::get<uint64_t>(headerUtfTable[0]["ContentOffset"]));
  }

  if (std::get<uint64_t>(headerUtfTable[0]["EtocOffset"]) != 0) {
    result->ReadEtoc(std::get<uint64_t>(headerUtfTable[0]["EtocOffset"]));
  }

  if (std::get<uint64_t>(headerUtfTable[0]["ItocOffset"]) != 0) {
    result->ReadItoc(std::get<uint64_t>(headerUtfTable[0]["ItocOffset"]),
                     std::get<uint64_t>(headerUtfTable[0]["ContentOffset"]),
                     alignVal);
  }

  for (uint32_t i = 0; i < result->FileCount; i++) {
    result->NamesToIds[result->FileList[i].FileName] = result->FileList[i].Id;
  }

  result->IsInit = true;
  *outArchive = result;
  return IoError_OK;
}

IoError CpkArchive::Open(FileMeta* file, Stream** outStream) {
  CpkMetaEntry* entry = (CpkMetaEntry*)file;

  IoError err;
  BaseStream->Seek(entry->Offset, RW_SEEK_SET);
  uint32_t const laylaMagic1 = 0x4352494C;
  uint32_t const laylaMagic2 = 0x41594C41;

  if (!entry->Compressed && ReadBE<uint32_t>(BaseStream) == laylaMagic1 &&
      ReadBE<uint32_t>(BaseStream) == laylaMagic2) {
    // Marking file as compressed because apparently some times it's just not
    // marked in toc
    entry->Size = ReadLE<uint32_t>(BaseStream) + 0x100;
    entry->Compressed = true;
  }

  if (entry->Compressed) {
    ImpLog(LogLevel::Debug, LogChannel::IO,
           "CPK cannot stream LAYLA compressed file \"{:s}\" in archive "
           "\"{:s}\"\n",
           entry->FileName, BaseStream->Meta.FileName);
    return IoError_Fail;
  } else {
    err = UncompressedStream::Create(BaseStream, entry->Offset, entry->Size,
                                     outStream);
  }
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "CPK file open failed for file \"{:s}\" in archive \"{:s}\"\n",
           entry->FileName, BaseStream->Meta.FileName);
  }
  return err;
}

// Based on https://forum.xentax.com/viewtopic.php?f=21&t=5137&hilit=CRILAYLA

static uint32_t get_next_bits(char* input, int* offset_p, uint32_t* bit_pool_p,
                              int* bits_left_p, int bit_count) {
  uint32_t out_bits = 0;

  if (*bits_left_p < bit_count) {
    int count = ((24 - *bits_left_p) >> 3) + 1;
    while (count) {
      uint8_t source = input[*offset_p];
      *offset_p = *offset_p - 1;
      *bit_pool_p = (*bit_pool_p << 8) | source;
      *bits_left_p += 8;
      count--;
    }
  }

  *bits_left_p -= bit_count;
  out_bits = *bit_pool_p >> *bits_left_p;
  uint32_t mask = (1 << bit_count) - 1;
  out_bits &= mask;

  return out_bits;
}

// Based on https://github.com/hcs64/vgm_ripping/tree/master/multi/utf_tab

static IoError DecompressLayla(char* input, int64_t compressedSize,
                               char* output, int64_t uncompressedSize) {
  int uncompressed_size = SDL_SwapLE32(*(uint32_t*)(input + 8));
  uint32_t compressedStreamLength = SDL_SwapLE32(*(uint32_t*)(input + 12));
  uint32_t compressedOffset = 16;
  int64_t prefixOffset = compressedOffset + compressedStreamLength;
  if (compressedSize < prefixOffset || compressedSize - prefixOffset != 0x100) {
    ImpLog(LogLevel::Debug, LogChannel::IO,
           "CPK unexpected end of LAYLA stream\n");
    return IoError_Fail;
  }
  memcpy(output, input + compressedOffset + compressedStreamLength, 0x100);

  int input_end = ((int)compressedSize - 0x100 - 1);
  int input_offset = input_end;
  int output_end = 0x100 + uncompressed_size - 1;
  uint32_t bit_pool = 0;
  int bits_left = 0, bytes_output = 0;
  int vle_lens[4] = {2, 3, 5, 8};

  while (bytes_output < uncompressed_size) {
    if (get_next_bits(input, &input_offset, &bit_pool, &bits_left, 1) > 0) {
      int backreference_offset =
          output_end - bytes_output +
          get_next_bits(input, &input_offset, &bit_pool, &bits_left, 13) + 3;
      int backreference_length = 3;
      int vle_level;

      for (vle_level = 0; vle_level < 4; vle_level++) {
        int this_level = get_next_bits(input, &input_offset, &bit_pool,
                                       &bits_left, vle_lens[vle_level]);
        backreference_length += this_level;
        if (this_level != ((1 << vle_lens[vle_level]) - 1)) break;
      }

      if (vle_level == 4) {
        int this_level;
        do {
          this_level =
              get_next_bits(input, &input_offset, &bit_pool, &bits_left, 8);
          backreference_length += this_level;
        } while (this_level == 255);
      }

      for (int i = 0; i < backreference_length; i++) {
        output[output_end - bytes_output] = output[backreference_offset--];
        bytes_output++;
      }
    } else {
      // verbatim byte
      output[output_end - bytes_output] = (uint8_t)get_next_bits(
          input, &input_offset, &bit_pool, &bits_left, 8);
      bytes_output++;
    }
  }
  return IoError_OK;
}

IoError CpkArchive::Slurp(FileMeta* file, void*& outBuffer, int64_t& outSize) {
  CpkMetaEntry* entry = (CpkMetaEntry*)file;
  if (!entry->Compressed) {
    return IoError_Fail;
  }

  int64_t pos = BaseStream->Seek(entry->Offset, RW_SEEK_SET);
  if (pos != entry->Offset) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "CPK failed to seek when slurping compressed file \"{:s}\" in "
           "archive \"{:s}\"\n",
           entry->FileName, BaseStream->Meta.FileName);
    return IoError_Fail;
  }
  char* compressedData = (char*)malloc(entry->CompressedSize);
  int64_t read = BaseStream->Read(compressedData, entry->CompressedSize);
  if (read != entry->CompressedSize) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "CPK failed to read compressed data when slurping compressed file "
           "\"{:s}\" in "
           "archive \"{:s}\"\n",
           entry->FileName, BaseStream->Meta.FileName);
    free(compressedData);
    return IoError_Fail;
  }

  outSize = entry->Size;
  outBuffer = malloc(outSize);

  IoError err = DecompressLayla(compressedData, entry->CompressedSize,
                                (char*)outBuffer, outSize);
  if (err != IoError_OK) free(outBuffer);
  free(compressedData);
  return err;
}

}  // namespace Io
}  // namespace Impacto