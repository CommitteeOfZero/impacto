#include "vfs.h"
#include "../impacto.h"
#include "../util.h"
#include <vector>
#include <mutex>
#include "vfsarchive.h"
#include "memorystream.h"
#include "../log.h"
#include "../profile/vfs.h"
#ifndef IMPACTO_DISABLE_MMAP
#include "memorymappedfilestream.h"
#else
#include "physicalfilestream.h"
#endif

#include "afsarchive.h"
#include "cpkarchive.h"
#include "lnk4archive.h"
#include "mpkarchive.h"
#include "textarchive.h"

namespace Impacto {
namespace Io {

template <typename T>
concept FileId = std::convertible_to<T, uint32_t> ||
                 std::convertible_to<T, std::string const&>;

using VfsArchiveFactory = auto (*)(Stream* stream, VfsArchive** outArchive)
    -> IoError;

static std::vector<VfsArchiveFactory> Archivers;
static ankerl::unordered_dense::map<std::string,
                                    std::vector<std::unique_ptr<VfsArchive>>,
                                    string_hash, std::equal_to<>>
    Mounts;
static std::shared_mutex Lock;

static IoError MountInternal(std::string const& mountpoint, Stream* stream) {
  VfsArchive* archive;
  IoError err = IoError_Fail;
  for (auto archiver : Archivers) {
    err = archiver(stream, &archive);
    if (err == IoError_OK) break;
  }
  if (err == IoError_OK) {
    archive->MountPoint = mountpoint;
    Mounts[mountpoint].emplace_back(archive);
  } else {
    ImpLog(LL_Error, LC_IO, "No archiver supports file {:s}\n",
           stream->Meta.FileName);
  }
  return err;
}

static VfsArchive* FindArchive(std::string const& mountpoint,
                               std::string const& fileName) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return 0;
  for (auto& archive : it->second) {
    if (archive->BaseStream->Meta.FileName == fileName) return archive.get();
  }
  return 0;
}

void VfsInit() {
  Archivers.push_back(AfsArchive::Create);
  Archivers.push_back(CpkArchive::Create);
  Archivers.push_back(Lnk4Archive::Create);
  Archivers.push_back(MpkArchive::Create);
  Archivers.push_back(TextArchive::Create);

  Profile::Vfs::Configure();
}

IoError VfsMount(std::string const& mountpoint,
                 std::string const& archiveFileName) {
  ImpLog(LL_Debug, LC_IO, "Trying to mount \"{:s}\" on mountpoint \"{:s}\"\n",
         archiveFileName, mountpoint);

  std::unique_lock mountLock{Lock};
  if (FindArchive(mountpoint, archiveFileName) != 0) {
    ImpLog(LL_Error, LC_IO, "File with this name already mounted!\n");
    return IoError_Fail;
  }

  Stream* archiveFile;
  IoError err;
#ifndef IMPACTO_DISABLE_MMAP
  err = MemoryMappedFileStream<AccessMode::read>::Create(archiveFileName,
                                                         &archiveFile);
#else
  err = PhysicalFileStream::Create(archiveFileName, &archiveFile);
#endif
  if (err != IoError_OK) {
    ImpLog(LL_Debug, LC_IO, "Could not open physical file \"{:s}\"\n",
           archiveFileName);
    return err;
  }
  err = MountInternal(mountpoint, archiveFile);
  if (err != IoError_OK) {
    delete archiveFile;
  }
  return err;
}

IoError VfsMountMemory(std::string const& mountpoint,
                       std::string const& archiveFileName, void* memory,
                       int64_t size, bool freeOnClose) {
  IoError err;
  Stream* archiveFile;

  ImpLog(LL_Debug, LC_IO,
         "Trying to mount in-memory archive named \"{:s}\" on mountpoint "
         "\"{:s}\"\n",
         archiveFileName, mountpoint);

  std::unique_lock mountLock{Lock};
  if (FindArchive(mountpoint, archiveFileName) != 0) {
    err = IoError_Fail;
    if (freeOnClose) free(memory);
    return err;
  }

  archiveFile = new MemoryStream(memory, size, freeOnClose);
  archiveFile->Meta.FileName = archiveFileName;
  err = MountInternal(mountpoint, archiveFile);
  if (err != IoError_OK) {
    delete archiveFile;
  }
  return err;
}

IoError VfsUnmount(std::string const& mountpoint,
                   std::string const& archiveFileName) {
  ImpLog(LL_Debug, LC_IO,
         "Trying to unmount archive named \"{:s}\" on mountpoint \"{:s}\"\n",
         archiveFileName, mountpoint);
  std::unique_lock unmountLock{Lock};
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return IoError_NotFound;
  for (auto arcIt = it->second.begin(); arcIt != it->second.end(); arcIt++) {
    if ((*arcIt)->BaseStream->Meta.FileName == archiveFileName) {
      it->second.erase(arcIt);
      return IoError_OK;
    }
  }
  ImpLog(LL_Debug, LC_IO, "Unmounting failed (file not mounted?)\n");
  return IoError_NotFound;
}

static IoError GetOrigMetaInternal(std::string const& mountpoint,
                                   std::string const& fileName,
                                   FileMeta*& outMeta,
                                   VfsArchive*& outArchive) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return IoError_NotFound;

  for (auto& archive : it->second) {
    auto nameToId = archive->NamesToIds.find(fileName);
    if (nameToId != archive->NamesToIds.end()) {
      outMeta = archive->IdsToFiles[nameToId->second];
      outArchive = archive.get();
      return IoError_OK;
    }
  }
  return IoError_NotFound;
}

static IoError GetOrigMetaInternal(std::string const& mountpoint, uint32_t id,
                                   FileMeta*& outMeta,
                                   VfsArchive*& outArchive) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return IoError_NotFound;

  for (auto& archive : it->second) {
    auto idToFile = archive->IdsToFiles.find(id);
    if (idToFile != archive->IdsToFiles.end()) {
      outMeta = idToFile->second;
      outArchive = archive.get();
      return IoError_OK;
    }
  }
  return IoError_NotFound;
}

template <FileId T>
IoError VfsGetMetaImpl(std::string const& mountpoint, T file,
                       FileMeta* outMeta) {
  IoError err;

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to get metadata for file \"{}\" on mountpoint \"{:s}\"\n",
             file, mountpoint);

  FileMeta* origMeta;
  VfsArchive* archive;
  std::shared_lock lock{Lock};
  err = GetOrigMetaInternal(mountpoint, file, origMeta, archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    return err;
  }

  *outMeta = *origMeta;
  outMeta->ArchiveMountPoint = mountpoint;
  outMeta->ArchiveFileName = archive->BaseStream->Meta.FileName;
  if (outMeta->Size < 0) {
    err = archive->GetCurrentSize(origMeta, outMeta->Size);
    if (err != IoError_OK) {
      ImpLog(LL_Error, LC_IO, "Getting current size failed!\n");
      return err;
    }
  }
  return err;
}

static IoError OpenInternal(std::string const& mountpoint, VfsArchive* archive,
                            FileMeta* origMeta, Stream** outStream) {
  IoError err;

  ImpLogSlow(LL_Debug, LC_IO,
             "Opening \"{:s}\" ({:d}) from mountpoint \"{:s}\" (archive file "
             "\"{:s}\")\n",
             origMeta->FileName, origMeta->Id, mountpoint,
             archive->BaseStream->Meta.FileName);

  err = archive->Open(origMeta, outStream);
  if (err != IoError_OK) {
    ImpLogSlow(LL_Debug, LC_IO, "VfsArchive->Open() failed, trying slurp\n");
    // maybe streaming is not supported
    void* memory;
    int64_t size;
    err = archive->Slurp(origMeta, memory, size);
    // TODO lazy slurp stream
    *outStream = new MemoryStream(memory, size, true);
  }
  if (err == IoError_OK) {
    (*outStream)->Meta.ArchiveFileName = archive->BaseStream->Meta.FileName;
    (*outStream)->Meta.ArchiveMountPoint = mountpoint;
    (*outStream)->Meta.FileName = origMeta->FileName;
    (*outStream)->Meta.Id = origMeta->Id;
  } else {
    ImpLog(LL_Error, LC_IO,
           "Opening \"{:s}\" ({:d}) from mountpoint \"{:s}\" (archive file "
           "\"{:s}\") "
           "failed\n",
           origMeta->FileName, origMeta->Id, mountpoint,
           archive->BaseStream->Meta.FileName);
  }
  return err;
}

template <FileId T>
IoError VfsOpenImpl(std::string const& mountpoint, T file, Stream** outStream) {
  IoError err;

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to open file \"{}\" on mountpoint \"{:s}\"\n", file,
             mountpoint);
  FileMeta* origMeta;
  VfsArchive* archive;
  std::unique_lock lock{Lock};
  err = GetOrigMetaInternal(mountpoint, file, origMeta, archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    return err;
  }

  return OpenInternal(mountpoint, archive, origMeta, outStream);
}

IoError SlurpInternal(VfsArchive* archive, FileMeta* origMeta, void*& outMemory,
                      int64_t& outSize) {
  IoError err;

  ImpLogSlow(LL_Debug, LC_IO,
             "Slurping \"{:s}\" ({:d}) from mountpoint \"{:s}\" (archive file "
             "\"{:s}\")\n",
             origMeta->FileName, origMeta->Id, archive->MountPoint,
             archive->BaseStream->Meta.FileName);

  err = archive->Slurp(origMeta, outMemory, outSize);
  if (err != IoError_OK) {
    ImpLogSlow(LL_Debug, LC_IO, "VfsArchive->Slurp() failed, trying open\n");

    Stream* stream;
    err = archive->Open(origMeta, &stream);
    if (err != IoError_OK) return err;
    outMemory = malloc(stream->Meta.Size);
    outSize = stream->Meta.Size;
    int64_t readErr = stream->Read(outMemory, outSize);
    // TODO should size output by Read() go into outSize, even though it may
    // be less than the allocated size?
    delete stream;
    if (readErr < 0) {
      free(outMemory);
      ImpLog(
          LL_Error, LC_IO,
          "Opening/reading \"{:s}\" ({:d}) from mountpoint \"{:s}\" (archive "
          "file "
          "\"{:s}\") failed\n",
          origMeta->FileName, origMeta->Id, archive->MountPoint,
          archive->BaseStream->Meta.FileName);
      err = IoError_Fail;
    } else {
      err = IoError_OK;
    }
  }
  return err;
}

template <FileId T>
IoError VfsSlurpImpl(std::string const& mountpoint, T fileName,
                     void*& outMemory, int64_t& outSize) {
  IoError err;
  std::unique_lock lock{Lock};
  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, fileName, origMeta, archive);
  if (err != IoError_OK) return err;

  return SlurpInternal(archive, origMeta, outMemory, outSize);
}

IoError VfsListFiles(std::string const& mountpoint,
                     std::map<uint32_t, std::string>& outListing) {
  IoError err;
  std::shared_lock lock{Lock};

  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) {
    err = IoError_NotFound;
    return err;
  }

  outListing.clear();

  // Reverse order so things first in the search path get overwritten last
  for (auto arcIt = it->second.rbegin(); arcIt != it->second.rend(); arcIt++) {
    for (auto nameToId : (*arcIt)->NamesToIds) {
      outListing[nameToId.second] = nameToId.first;
    }
  }

  return IoError_OK;
}

IoError VfsGetMeta(std::string const& mountpoint, std::string const& fileName,
                   FileMeta* outMeta) {
  return VfsGetMetaImpl(mountpoint, fileName, outMeta);
}
IoError VfsGetMeta(std::string const& mountpoint, uint32_t id,
                   FileMeta* outMeta) {
  return VfsGetMetaImpl(mountpoint, id, outMeta);
}
IoError VfsOpen(std::string const& mountpoint, std::string const& fileName,
                Stream** outStream) {
  return VfsOpenImpl(mountpoint, fileName, outStream);
}
IoError VfsOpen(std::string const& mountpoint, uint32_t id,
                Stream** outStream) {
  return VfsOpenImpl(mountpoint, id, outStream);
}
IoError VfsSlurp(std::string const& mountpoint, std::string const& fileName,
                 void*& outMemory, int64_t& outSize) {
  return VfsSlurpImpl(mountpoint, fileName, outMemory, outSize);
}
IoError VfsSlurp(std::string const& mountpoint, uint32_t id, void*& outMemory,
                 int64_t& outSize) {
  return VfsSlurpImpl(mountpoint, id, outMemory, outSize);
}

}  // namespace Io
}  // namespace Impacto