#include "patch.h"
#include "profile.h"
#include "profile_internal.h"

namespace Impacto::Profile::Patch {
void Configure() {
  if (TryPushMember("Patch")) {
    Language = TryGetMember<char const*>("Language").value_or("Japanese");
    DateFormat = TryGetMember<DateFormatType>("DateFormat")
                     .value_or(DateFormatType::YMD);
    HasScriptedExitLogic =
        TryGetMember<bool>("HasScriptedExitLogic").value_or(false);
    HasDelusionMouseSupport =
        TryGetMember<bool>("HasDelusionMouseSupport").value_or(false);
    Pop();
  }
}
}  // namespace Impacto::Profile::Patch