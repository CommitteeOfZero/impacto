#include "vfs.h"
#include "profile_internal.h"
#include "../io/vfs.h"
#include "../io/fsfolderarchive.h"

namespace Impacto {
namespace Profile {
namespace Vfs {
void Configure() {
  EnsurePushMemberOfType("Vfs", LUA_TTABLE);

  {
    EnsurePushMemberOfType("Mounts", LUA_TTABLE);
    ForEachProfileTable<std::string>([](std::string const& name) {
      ForEachProfileArray([&name](size_t index) {
        if (lua_istable(LuaState, -1)) {
          auto path = TryGetMember<std::string>("Path");
          if (!path) {
            std::string error = fmt::format(
                "Mount table for \"{:s}\" (index {:d}) is missing \"Path\"",
                name, index);
            ImpLog(LogLevel::Fatal, LogChannel::Profile, "{:s}\n", error);
            throw std::runtime_error(error);
          }

          std::optional<Io::FolderArchiveParameters> parameters =
              EnsureGetArrayElement<Io::FolderArchiveParameters>();

          Io::VfsMount(name, *path, parameters);
        } else {
          std::string file(EnsureGetArrayElement<std::string>());
          Io::VfsMount(name, file);
        }
      });
    });
    Pop();
  }

  Pop();
}
}  // namespace Vfs
}  // namespace Profile
}  // namespace Impacto