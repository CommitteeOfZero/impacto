#include "vfs.h"
#include "profile_internal.h"
#include "../io/vfs.h"

namespace Impacto {
namespace Profile {
namespace Vfs {
void Configure() {
  AssertHasMember(Json, "/", "Vfs");
  auto const& _vfs = Json["Vfs"];
  AssertIs(_vfs, "/Vfs", kObjectType);
  AssertHasMember(_vfs, "/Vfs", "Mounts");
  auto const& _mounts = _vfs["Mounts"];
  AssertIs(_mounts, "/Vfs/Mounts", kObjectType);

  for (Value::ConstMemberIterator it = _mounts.MemberBegin();
       it != _mounts.MemberEnd(); ++it) {
    AssertIs(it->name, "/Vfs/Mounts/x (name)", kStringType);
    AssertIs(it->value, "/Vfs/Mounts/x", kArrayType);
    char const* name = it->name.GetString();
    std::string _name(name);
    for (Value::ConstValueIterator archive = it->value.Begin();
         archive != it->value.End(); ++archive) {
      AssertIs(*archive, "/Vfs/Mounts/x/y", kStringType);
      Io::VfsMount(_name, std::string(archive->GetString()));
    }
  }
}
}  // namespace Vfs
}  // namespace Profile
}  // namespace Impacto