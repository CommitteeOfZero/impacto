#include "tipssystem.h"
#include "../profile_internal.h"

#include "../../games/mo6tw/tipssystem.h"
#include "../../games/chlcc/tipssystem.h"
#include "../../games/cclcc/tipssystem.h"

namespace Impacto {
namespace Profile {
namespace TipsSystem {

using namespace Impacto::TipsSystem;

void Configure() {
  EnsurePushMemberOfType("TipsSystem", LUA_TTABLE);

  Type = TipsSystemType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

  if (Type != +TipsSystemType::None) {
    MaxTipsCount = EnsureGetMember<int>("MaxTipsCount");

    switch (Type) {
      case TipsSystemType::MO6TW:
        Implementation = new Impacto::MO6TW::TipsSystem(MaxTipsCount);
        break;
      case TipsSystemType::CHLCC:
        Implementation = new Impacto::CHLCC::TipsSystem(MaxTipsCount);
        break;
      case TipsSystemType::CCLCC:
        Implementation = new Impacto::CCLCC::TipsSystem(MaxTipsCount);
        break;
      case TipsSystemType::None:
        ImpLog(LogLevel::Warning, LogChannel::Profile,
               "Tips system type is None, not setting system implementation\n");
    }
  }

  Pop();
}

}  // namespace TipsSystem
}  // namespace Profile
}  // namespace Impacto