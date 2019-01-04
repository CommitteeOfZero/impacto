#pragma once

#include <vector>

#include <SDL_rwops.h>

#include "../impacto.h"
#include "io.h"

// only these for new vfs
#include "inputstream.h"
#include <flat_hash_map.hpp>
#include <map>

namespace Impacto {
namespace Io {

// For transitional purposes, I'm keeping *both* VFS implementations here for
// now.

// TODO:
// - c0data style redirection (multiple source mountpoints -> one target)
// - Make the rest of the engine use the new VFS
// - Configurable physical file search paths
// - Search path reordering: For models, first mounted is good, we can configure
// static overrides, the model CPKs get mounted later. For other things, we
// might want to mount a patch archive when a user changes a setting in-game.

// The public interface of vfs.h is threadsafe. Individual InputStreams are not.
// Duplicate() them if you need to use them on multiple threads.

void VfsInit();
// Mount an archive from a physical file.
// Files will always be loaded from the earliest-mounted archive they're found
// in
IoError VfsMount(std::string const& mountpoint,
                 std::string const& archiveFileName);
// Mount an archive from memory. A unique filename must be specified to identify
// files coming from this archive and to unmount it.
IoError VfsMountMemory(std::string const& mountpoint,
                       std::string const& archiveFileName, void* memory,
                       int64_t size, bool freeOnClose);
// archiveFileName must match the filename an archive was mounted with
IoError VfsUnmount(std::string const& mountpoint,
                   std::string const& archiveFileName);
IoError VfsGetMeta(std::string const& mountpoint, std::string const& fileName,
                   FileMeta* outMeta);
IoError VfsGetMeta(std::string const& mountpoint, uint32_t id,
                   FileMeta* outMeta);
IoError VfsOpen(std::string const& mountpoint, std::string const& fileName,
                InputStream** outStream);
IoError VfsOpen(std::string const& mountpoint, uint32_t id,
                InputStream** outStream);
IoError VfsSlurp(std::string const& mountpoint, std::string const& fileName,
                 void** outMemory, int64_t* outSize);
IoError VfsSlurp(std::string const& mountpoint, uint32_t id, void** outMemory,
                 int64_t* outSize);
// You can provide a filled outListing, we'll clear it
IoError VfsListFiles(std::string const& mountpoint,
                     std::map<uint32_t, std::string>& outListing);

}  // namespace Io
}  // namespace Impacto