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

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      std::string name(EnsureGetKeyString());

      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        std::string file(EnsureGetArrayElementString());
        Io::VfsMount(name, file);
        Pop();
      }

      Pop();
    }

    Pop();
  }

  Pop();
}
}  // namespace Vfs
}  // namespace Profile
}  // namespace Impacto