#include "tipssystem.h"
#include "../profile_internal.h"

#include "../../games/mo6tw/tipssystem.h"
#include "../../games/chlcc/tipssystem.h"

namespace Impacto {
namespace Profile {
namespace TipsSystem {

using namespace Impacto::TipsSystem;

void Configure() {
  EnsurePushMemberOfType("TipsSystem", LUA_TTABLE);

  Type = TipsSystemType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  if (Type != +TipsSystemType::None) {
    MaxTipsCount = EnsureGetMemberInt("MaxTipsCount");
    MaxTipDataSize = EnsureGetMemberUint("MaxTipDataSize");

    switch (Type) {
      case TipsSystemType::MO6TW:
        Implementation = new Impacto::MO6TW::TipsSystem();
        break;
      case TipsSystemType::CHLCC:
        Implementation = new Impacto::CHLCC::TipsSystem();
        break;
      case TipsSystemType::None:
        ImpLog(LL_Warning, LC_Profile,
               "Tips system type is None, not setting system implementation\n");
    }
  }

  Pop();
}

}  // namespace TipsSystem
}  // namespace Profile
}  // namespace Impacto