#include "vfs.h"
#include "profile_internal.h"
#include "../io/vfs.h"

namespace Impacto {
namespace Profile {
namespace Vfs {
void Configure() {
  EnsurePushMemberOfType("Vfs", LUA_TTABLE);

  {
    EnsurePushMemberOfType("Mounts", LUA_TTABLE);
    ForEachProfileTable<std::string>([](std::string const& name) {
      ForEachProfileArray([&name](size_t index) {
        std::string file(EnsureGetArrayElement<std::string>());
        Io::VfsMount(name, file);
      });
    });
    Pop();
  }

  Pop();
}
}  // namespace Vfs
}  // namespace Profile
}  // namespace Impacto