#pragma once

#include <vector>

#include "../impacto.h"
#include "io.h"

// only these for new vfs
#include "stream.h"
#include <ankerl/unordered_dense.h>
#include <map>

namespace Impacto {
namespace Io {
// The public interface of vfs.h is threadsafe. Individual Streams are not.
// Duplicate() them if you need to use them on multiple threads.

void VfsInit();
// Mount an archive from a physical file.
// Archives with the same name can be mounted several times, files will always
// be loaded from the latest-mounted archive they're found in
IoError VfsMount(std::string const& mountpoint,
                 std::string const& archiveFileName);
// Mount an archive from memory. A unique filename must be specified to identify
// files coming from this archive and to unmount it.
IoError VfsMountMemory(std::string const& mountpoint,
                       std::string const& archiveFileName, void* memory,
                       int64_t size, bool freeOnClose,
                       bool invertMountOrder = false);
// archiveFileName must match the filename an archive was mounted with
IoError VfsUnmount(std::string const& mountpoint,
                   std::string const& archiveFileName);
IoError VfsGetMeta(std::string const& mountpoint, std::string const& fileName,
                   FileMeta* outMeta);
IoError VfsGetMeta(std::string const& mountpoint, uint32_t id,
                   FileMeta* outMeta);
IoError VfsOpen(std::string const& mountpoint, std::string const& fileName,
                Stream** outStream);
IoError VfsOpen(std::string const& mountpoint, uint32_t id, Stream** outStream);
IoError VfsSlurp(std::string const& mountpoint, std::string const& fileName,
                 void*& outMemory, int64_t& outSize);
IoError VfsSlurp(std::string const& mountpoint, uint32_t id, void*& outMemory,
                 int64_t& outSize);
// You can provide a filled outListing, we'll clear it
IoError VfsListFiles(std::string const& mountpoint,
                     std::map<uint32_t, std::string>& outListing);

}  // namespace Io
}  // namespace Impacto