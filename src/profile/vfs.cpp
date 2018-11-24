#include "vfs.h"
#include "profile_internal.h"
#include "../io/vfs.h"

namespace Impacto {
namespace Profile {
namespace Vfs {
void Configure() {
  auto const& _mounts = EnsureGetMemberOfType(
      EnsureGetMemberOfType(Json, "/", "Vfs", kObjectType), "/Vfs", "Mounts",
      kObjectType);

  for (Value::ConstMemberIterator it = _mounts.MemberBegin();
       it != _mounts.MemberEnd(); ++it) {
    AssertIs(it->value, "/Vfs/Mounts/x", kArrayType);
    std::string name(EnsureGetString(it->name, "/Vfs/Mounts/x (name)"));
    for (Value::ConstValueIterator archive = it->value.Begin();
         archive != it->value.End(); ++archive) {
      std::string file(EnsureGetString(*archive, "/Vfs/Mounts/x/y"));
      Io::VfsMount(name, file);
    }
  }
}
}  // namespace Vfs
}  // namespace Profile
}  // namespace Impacto