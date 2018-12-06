#include "vfs.h"
#include "profile_internal.h"
#include "../io/vfs.h"

namespace Impacto {
namespace Profile {
namespace Vfs {
void Configure() {
  EnsurePushMemberOfType("Vfs", kObjectType);

  {
    EnsurePushMemberOfType("Mounts", kObjectType);

    auto const& _mounts = TopVal();

    for (Value::ConstMemberIterator it = _mounts.MemberBegin();
         it != _mounts.MemberEnd(); it++) {
      std::string name(EnsureGetKeyString(it));

      EnsurePushMemberIteratorOfType(it, kArrayType);

      uint32_t mountCount = TopVal().Size();
      for (uint32_t i = 0; i < mountCount; i++) {
        std::string file(EnsureGetArrayElementString(i));
        Io::VfsMount(name, file);
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