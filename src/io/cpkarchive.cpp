#include "cpkarchive.h"

#include "../log.h"
#include "uncompressedstream.h"
#include <SDL_endian.h>

namespace Impacto {
namespace Io {

enum CpkColumnFlags {
  STORAGE_MASK = 0xf0,
  STORAGE_NONE = 0x00,
  STORAGE_ZERO = 0x10,
  STORAGE_CONSTANT = 0x30,
  STORAGE_PERROW = 0x50,

  TYPE_MASK = 0x0f,
  TYPE_DATA = 0x0b,
  TYPE_STRING = 0x0a,
  TYPE_FLOAT = 0x08,
  TYPE_8BYTE2 = 0x07,
  TYPE_8BYTE = 0x06,
  TYPE_4BYTE2 = 0x05,
  TYPE_4BYTE = 0x04,
  TYPE_2BYTE2 = 0x03,
  TYPE_2BYTE = 0x02,
  TYPE_1BYTE2 = 0x01,
  TYPE_1BYTE = 0x00,
};

struct CpkColumn {
  uint32_t Flags;
  char Name[CpkMaxPath];
  std::vector<CpkCell> Cells;
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
    uint8_t* utfBlock, uint64_t utfSize,
    std::vector<ska::flat_hash_map<std::string, CpkCell>>* rows) {
  const uint32_t utfMagic = 0x40555446;
  UtfStream = new MemoryStream(utfBlock, utfSize, true);
  if (ReadBE<uint32_t>(UtfStream) != utfMagic) {
    DecryptUtfBlock(utfBlock, utfSize);
    UtfStream->Seek(0, RW_SEEK_SET);
    if (ReadBE<uint32_t>(UtfStream) != utfMagic) {
      ImpLog(LL_Trace, LC_IO, "Error reading CPK UTF table\n");
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

  for (int i = 0; i < numColumns; i++) {
    CpkColumn column;
    column.Flags = ReadU8(UtfStream);
    if (column.Flags == 0) column.Flags = ReadBE<uint32_t>(UtfStream);

    ReadString(stringsOffset, column.Name);

    columns.push_back(column);
  }

  for (int i = 0; i < numRows; i++) {
    UtfStream->Seek(rowsOffset + (i * rowLength), RW_SEEK_SET);
    ska::flat_hash_map<std::string, CpkCell> row;
    for (auto& column : columns) {
      CpkCell cell;
      int storageFlag = column.Flags & CpkColumnFlags::STORAGE_MASK;
      if (storageFlag == 0x50) {
        int cellType = column.Flags & CpkColumnFlags::TYPE_MASK;
        switch (cellType) {
          case 0:
          case 1:
            cell.Uint8Val = ReadU8(UtfStream);
            break;
          case 2:
          case 3:
            cell.Uint16Val = ReadBE<uint16_t>(UtfStream);
            break;
          case 4:
          case 5:
            cell.Uint32Val = ReadBE<uint32_t>(UtfStream);
            break;
          case 6:
          case 7:
            cell.Uint64Val = ReadBE<uint64_t>(UtfStream);
            break;
          case 8:
            cell.FloatVal = ReadBE<float>(UtfStream);
            break;
          case 0xA:
            ReadString(stringsOffset, cell.StringVal);
            break;
          case 0xB:
            int64_t dataPos = ReadBE<uint32_t>(UtfStream) + dataOffset;
            uint64_t dataSize = ReadBE<uint32_t>(UtfStream);
            uint64_t retAddr = UtfStream->Position;
            cell.DataArray = (uint8_t*)malloc(dataSize + sizeof(uint64_t));
            cell.DataSize = dataSize;
            if (cell.DataArray != 0) {
              UtfStream->Seek(dataPos, RW_SEEK_SET);
              UtfStream->Read(cell.DataArray, dataSize);
              UtfStream->Seek(retAddr, RW_SEEK_SET);
            }
            break;
        }
      } else {
        cell.Uint64Val = 0;
      }
      row[column.Name] = cell;
    }
    rows->push_back(row);
  }

  delete UtfStream;
  return true;
}

void CpkArchive::ReadString(int64_t stringsOffset, char* output) {
  int64_t stringAddr = stringsOffset + ReadBE<uint32_t>(UtfStream);

  int64_t retAddr = UtfStream->Position;

  UtfStream->Seek(stringAddr, RW_SEEK_SET);

  memset(output, 0, CpkMaxPath);

  char ch;
  int i = 0;
  while ((ch = ReadU8(UtfStream)) != 0x00) {
    output[i++] = ch;
  }
  output[i] = '\0';
  UtfStream->Seek(retAddr, RW_SEEK_SET);
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
    ImpLog(LL_Trace, LC_IO, "Error reading CPK ITOC\n");
    return IoError_Fail;
  }
  BaseStream->Seek(4, RW_SEEK_CUR);
  uint64_t utfSize = ReadLE<uint64_t>(BaseStream);
  uint8_t* utfBlock = (uint8_t*)malloc(utfSize);
  BaseStream->Read(utfBlock, utfSize);
  std::vector<ska::flat_hash_map<std::string, CpkCell>> itocUtfTable;
  if (!ReadUtfBlock(utfBlock, utfSize, &itocUtfTable)) return IoError_Fail;

  if (itocUtfTable[0]["DataL"].Uint64Val != 0) {
    std::vector<ska::flat_hash_map<std::string, CpkCell>> dataLUtfTable;
    if (!ReadUtfBlock(itocUtfTable[0]["DataL"].DataArray,
                      itocUtfTable[0]["DataL"].DataSize, &dataLUtfTable))
      return IoError_Fail;
    std::vector<ska::flat_hash_map<std::string, CpkCell>> dataHUtfTable;
    if (!ReadUtfBlock(itocUtfTable[0]["DataH"].DataArray,
                      itocUtfTable[0]["DataH"].DataSize, &dataHUtfTable))
      return IoError_Fail;

    for (auto& row : dataLUtfTable) {
      int id = row["ID"].Uint16Val;
      CpkMetaEntry* entry = GetFileListEntry(id);
      entry->Id = id;

      uint16_t extractedSize = row["ExtractSize"].Uint16Val;
      uint16_t fileSize = row["FileSize"].Uint16Val;
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
      int id = row["ID"].Uint16Val;
      CpkMetaEntry* entry = GetFileListEntry(id);
      entry->Id = id;

      int extractedSize = row["ExtractSize"].Uint32Val;
      int fileSize = row["FileSize"].Uint32Val;
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
  for (const auto kv : fileVec) {
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
    ImpLog(LL_Trace, LC_IO, "Error reading CPK TOC\n");
    return IoError_Fail;
  }
  BaseStream->Seek(4, RW_SEEK_CUR);
  uint64_t utfSize = ReadLE<uint64_t>(BaseStream);
  uint8_t* utfBlock = (uint8_t*)malloc(utfSize);
  BaseStream->Read(utfBlock, utfSize);
  std::vector<ska::flat_hash_map<std::string, CpkCell>> tocUtfTable;
  if (!ReadUtfBlock(utfBlock, utfSize, &tocUtfTable)) return IoError_Fail;

  for (auto& row : tocUtfTable) {
    uint32_t id = row["ID"].Uint32Val;
    CpkMetaEntry* entry = GetFileListEntry(id);

    entry->Id = id;
    entry->Offset = row["FileOffset"].Uint64Val;

    char path[CpkMaxPath * 2] = {0};

    if (*row["DirName"].StringVal) {
      snprintf(path, CpkMaxPath * 2, "%s/%s", row["DirName"].StringVal,
               row["FileName"].StringVal);
    } else {
      snprintf(path, CpkMaxPath, "%s", row["FileName"].StringVal);
    }
    if (strlen(path) == 0) {
      snprintf(path, CpkMaxPath, "%05i", id);
    }
    entry->FileName = path;

    int extractedSize = row["ExtractSize"].Uint32Val;
    int fileSize = row["FileSize"].Uint32Val;
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
    ImpLog(LL_Trace, LC_IO, "Error reading CPK ETOC\n");
    return IoError_Fail;
  }
  BaseStream->Seek(4, RW_SEEK_CUR);
  uint64_t utfSize = ReadLE<uint64_t>(BaseStream);
  uint8_t* utfBlock = (uint8_t*)malloc(utfSize);
  BaseStream->Read(utfBlock, utfSize);
  std::vector<ska::flat_hash_map<std::string, CpkCell>> etocUtfTable;
  if (!ReadUtfBlock(utfBlock, utfSize, &etocUtfTable)) return IoError_Fail;

  // for (auto& row : etocUtfTable) {
  // TODO: This contains the LocalDir and UpdateDateTime params. Do we actually
  // need this?...
  //}

  return IoError_OK;
}

IoError CpkArchive::Create(InputStream* stream, VfsArchive** outArchive) {
  ImpLog(LL_Trace, LC_IO, "Trying to mount \"%s\" as CPK\n",
         stream->Meta.FileName.c_str());

  CpkArchive* result = 0;

  std::vector<ska::flat_hash_map<std::string, CpkCell>> headerUtfTable;

  uint16_t alignVal;
  uint64_t utfSize = 0;
  uint8_t* utfBlock = 0;

  uint32_t const magic = 0x43504B20;
  if (ReadBE<uint32_t>(stream) != magic) {
    ImpLog(LL_Trace, LC_IO, "Not a CPK\n");
    goto fail;
  }

  result = new CpkArchive;
  result->BaseStream = stream;

  stream->Seek(0x8, RW_SEEK_SET);
  utfSize = ReadLE<uint64_t>(stream);
  utfBlock = (uint8_t*)malloc(utfSize);
  stream->Read(utfBlock, utfSize);
  if (!result->ReadUtfBlock(utfBlock, utfSize, &headerUtfTable)) {
    goto fail;
  }

  alignVal = headerUtfTable[0]["Align"].Uint16Val;
  result->FileCount = headerUtfTable[0]["Files"].Uint32Val;
  result->Version = headerUtfTable[0]["Version"].Uint16Val;
  result->Revision = headerUtfTable[0]["Revision"].Uint16Val;

  result->FileList = new CpkMetaEntry[result->FileCount];

  if (headerUtfTable[0]["TocOffset"].Uint64Val != 0) {
    result->ReadToc(headerUtfTable[0]["TocOffset"].Uint64Val,
                    headerUtfTable[0]["ContentOffset"].Uint64Val);
  }

  if (headerUtfTable[0]["EtocOffset"].Uint64Val != 0) {
    result->ReadEtoc(headerUtfTable[0]["EtocOffset"].Uint64Val);
  }

  if (headerUtfTable[0]["ItocOffset"].Uint64Val != 0) {
    result->ReadItoc(headerUtfTable[0]["ItocOffset"].Uint64Val,
                     headerUtfTable[0]["ContentOffset"].Uint64Val, alignVal);
  }

  for (int i = 0; i < result->FileCount; i++) {
    result->NamesToIds[result->FileList[i].FileName] = result->FileList[i].Id;
  }

  result->IsInit = true;
  *outArchive = result;
  return IoError_OK;

fail:
  stream->Seek(0, RW_SEEK_SET);
  if (result) delete result;
  return IoError_Fail;
}

IoError CpkArchive::Open(FileMeta* file, InputStream** outStream) {
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
    ImpLog(LL_Debug, LC_IO,
           "CPK cannot stream LAYLA compressed file \"%s\" in archive "
           "\"%s\"\n",
           entry->FileName.c_str(), BaseStream->Meta.FileName.c_str());
    return IoError_Fail;
  } else {
    err = UncompressedStream::Create(BaseStream, entry->Offset, entry->Size,
                                     outStream);
  }
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO,
           "CPK file open failed for file \"%s\" in archive \"%s\"\n",
           entry->FileName.c_str(), BaseStream->Meta.FileName.c_str());
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

static IoError DecompressLayla(char* input, uint32_t compressedSize,
                               char* output, uint32_t uncompressedSize) {
  int uncompressed_size = SDL_SwapLE32(*(uint32_t*)(input + 8));
  uint32_t compressedStreamLength = SDL_SwapLE32(*(uint32_t*)(input + 12));
  uint32_t compressedOffset = 16;
  uint32_t prefixOffset = compressedOffset + compressedStreamLength;
  if (compressedSize < prefixOffset || compressedSize - prefixOffset != 0x100) {
    ImpLog(LL_Debug, LC_IO, "CPK unexpected end of LAYLA stream\n");
    return IoError_Fail;
  }
  memcpy(output, input + compressedOffset + compressedStreamLength, 0x100);

  int input_end = (compressedSize - 0x100 - 1);
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

IoError CpkArchive::Slurp(FileMeta* file, void** outBuffer, int64_t* outSize) {
  CpkMetaEntry* entry = (CpkMetaEntry*)file;
  if (!entry->Compressed) {
    return IoError_Fail;
  }

  int64_t pos = BaseStream->Seek(entry->Offset, RW_SEEK_SET);
  if (pos != entry->Offset) {
    ImpLog(LL_Error, LC_IO,
           "CPK failed to seek when slurping compressed file \"%s\" in "
           "archive \"%s\"\n",
           entry->FileName.c_str(), BaseStream->Meta.FileName.c_str());
    return IoError_Fail;
  }
  char* compressedData = (char*)malloc(entry->CompressedSize);
  int64_t read = BaseStream->Read(compressedData, entry->CompressedSize);
  if (read != entry->CompressedSize) {
    ImpLog(LL_Error, LC_IO,
           "CPK failed to read compressed data when slurping compressed file "
           "\"%s\" in "
           "archive \"%s\"\n",
           entry->FileName.c_str(), BaseStream->Meta.FileName.c_str());
    free(compressedData);
    return IoError_Fail;
  }

  *outSize = entry->Size;
  *outBuffer = malloc(*outSize);

  IoError err = DecompressLayla(compressedData, entry->CompressedSize,
                                (char*)*outBuffer, *outSize);
  if (err != IoError_OK) free(*outBuffer);
  free(compressedData);
  return err;
}

}  // namespace Io
}  // namespace Impacto