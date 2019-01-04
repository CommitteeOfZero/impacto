#include "vfs.h"
#include "../impacto.h"
#include <vector>
#include "vfsarchive.h"
#include "physicalfilestream.h"
#include "memorystream.h"
#include "../log.h"
#include "../profile/vfs.h"

#include "afsarchive.h"
#include "cpkarchive.h"
#include "lnk4archive.h"
#include "mpkarchive.h"
#include "textarchive.h"

namespace Impacto {
namespace Io {

typedef IoError (*VfsArchiveFactory)(InputStream* stream,
                                     VfsArchive** outArchive);

static std::vector<VfsArchiveFactory> Archivers;
static ska::flat_hash_map<std::string, std::vector<VfsArchive*>> Mounts;
static SDL_mutex* Lock;

static IoError MountInternal(std::string const& mountpoint,
                             InputStream* stream) {
  VfsArchive* archive;
  IoError err = IoError_Fail;
  for (auto archiver : Archivers) {
    err = archiver(stream, &archive);
    if (err == IoError_OK) break;
  }
  if (err == IoError_OK) {
    archive->MountPoint = mountpoint;
    Mounts[mountpoint].push_back(archive);
  } else {
    ImpLog(LL_Error, LC_IO, "No archiver supports file %s\n",
           stream->Meta.FileName.c_str());
  }
  return err;
}

static VfsArchive* FindArchive(std::string const& mountpoint,
                               std::string const& fileName) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return 0;
  for (auto archive : it->second) {
    if (archive->BaseStream->Meta.FileName == fileName) return archive;
  }
  return 0;
}

void VfsInit() {
  Lock = SDL_CreateMutex();

  Archivers.push_back(AfsArchive::Create);
  Archivers.push_back(CpkArchive::Create);
  Archivers.push_back(Lnk4Archive::Create);
  Archivers.push_back(MpkArchive::Create);
  Archivers.push_back(TextArchive::Create);

  Profile::Vfs::Configure();
}

IoError VfsMount(std::string const& mountpoint,
                 std::string const& archiveFileName) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLog(LL_Debug, LC_IO, "Trying to mount \"%s\" on mountpoint \"%s\"\n",
         archiveFileName.c_str(), mountpoint.c_str());

  if (FindArchive(mountpoint, archiveFileName) != 0) {
    ImpLog(LL_Error, LC_IO, "File with this name already mounted!\n");
    err = IoError_Fail;
    goto end;
  }

  InputStream* archiveFile;
  err = PhysicalFileStream::Create(archiveFileName, &archiveFile);
  if (err != IoError_OK) {
    ImpLog(LL_Debug, LC_IO, "Could not open physical file \"%s\"\n",
           archiveFileName.c_str());
    goto end;
  }

  err = MountInternal(mountpoint, archiveFile);
  if (err != IoError_OK) {
    delete archiveFile;
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsMountMemory(std::string const& mountpoint,
                       std::string const& archiveFileName, void* memory,
                       int64_t size, bool freeOnClose) {
  IoError err;
  InputStream* archiveFile;

  SDL_LockMutex(Lock);

  ImpLog(
      LL_Debug, LC_IO,
      "Trying to mount in-memory archive named \"%s\" on mountpoint \"%s\"\n",
      archiveFileName.c_str(), mountpoint.c_str());

  if (FindArchive(mountpoint, archiveFileName) != 0) {
    err = IoError_Fail;
    if (freeOnClose) free(memory);
    goto end;
  }

  archiveFile = new MemoryStream(memory, size, freeOnClose);
  archiveFile->Meta.FileName = archiveFileName;
  err = MountInternal(mountpoint, archiveFile);
  if (err != IoError_OK) {
    delete archiveFile;
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsUnmount(std::string const& mountpoint,
                   std::string const& archiveFileName) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLog(LL_Debug, LC_IO,
         "Trying to unmount archive named \"%s\" on mountpoint \"%s\"\n",
         archiveFileName.c_str(), mountpoint.c_str());

  err = IoError_NotFound;
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) goto end;
  for (auto arcIt = it->second.begin(); arcIt != it->second.end(); arcIt++) {
    if ((*arcIt)->BaseStream->Meta.FileName == archiveFileName) {
      delete *arcIt;
      it->second.erase(arcIt);
      err = IoError_OK;
      break;
    }
  }
  if (err != IoError_OK) {
    ImpLog(LL_Debug, LC_IO, "Unmounting failed (file not mounted?)\n");
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

static IoError GetOrigMetaInternal(std::string const& mountpoint,
                                   std::string const& fileName,
                                   FileMeta** outMeta,
                                   VfsArchive** outArchive) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return IoError_NotFound;

  for (auto archive : it->second) {
    auto nameToId = archive->NamesToIds.find(fileName);
    if (nameToId != archive->NamesToIds.end()) {
      *outMeta = archive->IdsToFiles[nameToId->second];
      *outArchive = archive;
      return IoError_OK;
    }
  }
  return IoError_NotFound;
}

static IoError GetOrigMetaInternal(std::string const& mountpoint, uint32_t id,
                                   FileMeta** outMeta,
                                   VfsArchive** outArchive) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return IoError_NotFound;

  for (auto archive : it->second) {
    auto idToFile = archive->IdsToFiles.find(id);
    if (idToFile != archive->IdsToFiles.end()) {
      *outMeta = idToFile->second;
      *outArchive = archive;
      return IoError_OK;
    }
  }
  return IoError_NotFound;
}

IoError VfsGetMeta(std::string const& mountpoint, std::string const& fileName,
                   FileMeta* outMeta) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to get metadata for file \"%s\" on mountpoint \"%s\"\n",
             fileName.c_str(), mountpoint.c_str());

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, fileName, &origMeta, &archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    goto end;
  }

  *outMeta = *origMeta;
  outMeta->ArchiveMountPoint = mountpoint;
  outMeta->ArchiveFileName = archive->BaseStream->Meta.FileName;
  if (outMeta->Size < 0) {
    err = archive->GetCurrentSize(origMeta, &outMeta->Size);
    if (err != IoError_OK) {
      ImpLog(LL_Error, LC_IO, "Getting current size failed!\n");
      goto end;
    }
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsGetMeta(std::string const& mountpoint, uint32_t id,
                   FileMeta* outMeta) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to get metadata for file %d on mountpoint \"%s\"\n", id,
             mountpoint.c_str());

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, id, &origMeta, &archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    goto end;
  }

  *outMeta = *origMeta;
  outMeta->ArchiveMountPoint = mountpoint;
  outMeta->ArchiveFileName = archive->BaseStream->Meta.FileName;
  if (outMeta->Size < 0) {
    err = archive->GetCurrentSize(origMeta, &outMeta->Size);
    if (err != IoError_OK) {
      ImpLog(LL_Error, LC_IO, "Getting current size failed!\n");
      goto end;
    }
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

static IoError OpenInternal(std::string const& mountpoint, VfsArchive* archive,
                            FileMeta* origMeta, InputStream** outStream) {
  IoError err;

  ImpLogSlow(
      LL_Debug, LC_IO,
      "Opening \"%s\" (%d) from mountpoint \"%s\" (archive file \"%s\")\n",
      origMeta->FileName.c_str(), origMeta->Id, mountpoint.c_str(),
      archive->BaseStream->Meta.FileName.c_str());

  err = archive->Open(origMeta, outStream);
  if (err != IoError_OK) {
    ImpLogSlow(LL_Debug, LC_IO, "VfsArchive->Open() failed, trying slurp\n");
    // maybe streaming is not supported
    void* memory;
    int64_t size;
    err = archive->Slurp(origMeta, &memory, &size);
    // TODO lazy slurp stream
    *outStream = new MemoryStream(memory, size, true);
  }
  if (err == IoError_OK) {
    (*outStream)->Meta.ArchiveFileName = archive->BaseStream->Meta.FileName;
    (*outStream)->Meta.ArchiveMountPoint = mountpoint;
    (*outStream)->Meta.FileName = origMeta->FileName;
  } else {
    ImpLog(LL_Error, LC_IO,
           "Opening \"%s\" (%d) from mountpoint \"%s\" (archive file \"%s\") "
           "failed\n",
           origMeta->FileName.c_str(), origMeta->Id, mountpoint.c_str(),
           archive->BaseStream->Meta.FileName.c_str());
  }
  return err;
}

IoError VfsOpen(std::string const& mountpoint, std::string const& fileName,
                InputStream** outStream) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to open file \"%s\" on mountpoint \"%s\"\n",
             fileName.c_str(), mountpoint.c_str());

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, fileName, &origMeta, &archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    goto end;
  }

  err = OpenInternal(mountpoint, archive, origMeta, outStream);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsOpen(std::string const& mountpoint, uint32_t id,
                InputStream** outStream) {
  IoError err;
  SDL_LockMutex(Lock);

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, id, &origMeta, &archive);
  if (err != IoError_OK) goto end;

  err = OpenInternal(mountpoint, archive, origMeta, outStream);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError SlurpInternal(VfsArchive* archive, FileMeta* origMeta, void** outMemory,
                      int64_t* outSize) {
  IoError err;

  ImpLogSlow(
      LL_Debug, LC_IO,
      "Slurping \"%s\" (%d) from mountpoint \"%s\" (archive file \"%s\")\n",
      origMeta->FileName.c_str(), origMeta->Id, archive->MountPoint.c_str(),
      archive->BaseStream->Meta.FileName.c_str());

  err = archive->Slurp(origMeta, outMemory, outSize);
  if (err != IoError_OK) {
    ImpLogSlow(LL_Debug, LC_IO, "VfsArchive->Slurp() failed, trying open\n");

    InputStream* stream;
    err = archive->Open(origMeta, &stream);
    if (err != IoError_OK) return err;
    *outMemory = malloc(stream->Meta.Size);
    *outSize = stream->Meta.Size;
    int64_t readErr = stream->Read(*outMemory, *outSize);
    // TODO should size output by Read() go into outSize, even though it may be
    // less than the allocated size?
    delete stream;
    if (readErr < 0) {
      free(*outMemory);
      ImpLog(LL_Error, LC_IO,
             "Opening/reading \"%s\" (%d) from mountpoint \"%s\" (archive file "
             "\"%s\") failed\n",
             origMeta->FileName.c_str(), origMeta->Id,
             archive->MountPoint.c_str(),
             archive->BaseStream->Meta.FileName.c_str());
      err = IoError_Fail;
    } else {
      err = IoError_OK;
    }
  }
  return err;
}

IoError VfsSlurp(std::string const& mountpoint, std::string const& fileName,
                 void** outMemory, int64_t* outSize) {
  IoError err;
  SDL_LockMutex(Lock);

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, fileName, &origMeta, &archive);
  if (err != IoError_OK) goto end;

  err = SlurpInternal(archive, origMeta, outMemory, outSize);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsSlurp(std::string const& mountpoint, uint32_t id, void** outMemory,
                 int64_t* outSize) {
  IoError err;
  SDL_LockMutex(Lock);

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, id, &origMeta, &archive);
  if (err != IoError_OK) goto end;

  err = SlurpInternal(archive, origMeta, outMemory, outSize);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsListFiles(std::string const& mountpoint,
                     std::map<uint32_t, std::string>& outListing) {
  IoError err;
  SDL_LockMutex(Lock);

  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) err = IoError_NotFound;

  outListing.clear();

  // Reverse order so things first in the search path get overwritten last
  for (auto arcIt = it->second.rbegin(); arcIt != it->second.rend(); arcIt++) {
    for (auto nameToId : (*arcIt)->NamesToIds) {
      outListing[nameToId.second] = nameToId.first;
    }
  }

  err = IoError_OK;

end:
  SDL_UnlockMutex(Lock);
  return err;
}

}  // namespace Io
}  // namespace Impacto