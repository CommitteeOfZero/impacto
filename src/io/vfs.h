#pragma once

#include <SDL_rwops.h>

#include "../impacto.h"
#include "io.h"

namespace Impacto {

// Including null terminator
const int VfsMaxPath = 224;

// The overlay FS allows replacing individual files in an archive using the
// physical filesystem for quick tweaks, e.g.
// <VfsOverlayPath>/MODEL.CPK/c002_010.cpk/0000
extern bool VfsOverlayEnabled;
// Path to root of overlay FS relative to working directory
extern const char VfsOverlayPath[];
// Path to look for any asset archives in relative to working directory
extern const char VfsBasePath[];

struct VfsDriver;
struct VfsArchive;

struct VfsFileInfo {
  uint32_t Id;
  char Name[VfsMaxPath];
};

void VfsRegisterDriver(VfsDriver* driver);

IoError VfsMount(const char* archiveName, VfsArchive** outArchive);

// VfsMountChild loads the entire archive into memory
IoError VfsMountChild(VfsArchive* parent, uint32_t id, VfsArchive** outArchive);
IoError VfsMountChild(VfsArchive* parent, const char* path,
                      VfsArchive** outArchive);

// Recursively unmounts and frees an archive
IoError VfsUnmount(VfsArchive* archive);

// Reads whole file into malloc()'d buffer
// Data must be freed by caller if no error occurs
IoError VfsSlurp(VfsArchive* archive, uint32_t id, void** outData,
                 size_t* outSize);
IoError VfsSlurp(VfsArchive* archive, const char* path, void** outData,
                 size_t* outSize);

IoError VfsOpen(VfsArchive* archive, uint32_t id, SDL_RWops** outHandle);
IoError VfsOpen(VfsArchive* archive, const char* path, SDL_RWops** outHandle);

// outName must have space for VfsMaxPath characters (including null terminator)
IoError VfsGetName(VfsArchive* archive, uint32_t id, char* outName);
IoError VfsGetId(VfsArchive* archive, const char* path, uint32_t* outId);

IoError VfsGetSize(VfsArchive* archive, uint32_t id, size_t* outSize);
IoError VfsGetSize(VfsArchive* archive, const char* path, size_t* outSize);

// IDs are not required to be sequential...
// These return IoError_Eof if the last enumerated file is the last in the
// archive
IoError VfsEnumerateStart(VfsArchive* archive, uint32_t* outIterator,
                          VfsFileInfo* outFileInfo);
IoError VfsEnumerateNext(VfsArchive* archive, uint32_t* inoutIterator,
                         VfsFileInfo* outFileInfo);

}  // namespace Impacto