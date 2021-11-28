#include "tipssystem.h"
#include "../profile_internal.h"

#include "../../games/mo6tw/tipssystem.h"

namespace Impacto {
namespace Profile {
namespace TipsSystem {

using namespace Impacto::TipsSystem;

TipsSystemType Type = TipsSystemType::None;

int MaxTipsCount;
uint32_t MaxTipDataSize;

void Configure() {
  EnsurePushMemberOfType("TipsSystem", kObjectType);

  Type = TipsSystemType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  if (Type != +TipsSystemType::None) {
    MaxTipsCount = EnsureGetMemberInt("MaxTipsCount");
    MaxTipDataSize = EnsureGetMemberUint("MaxTipDataSize");

    switch (Type) {
      case TipsSystemType::MO6TW:
        Implementation = new Impacto::MO6TW::TipsSystem();
        break;
    }
  }

  Pop();
}

}  // namespace TipsSystem
}  // namespace Profile
}  // namespace Impacto