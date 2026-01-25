#include "menutransition.h"

#include "../../../profile/game.h"
#include "../../../profile/ui/gamespecific.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile;
using namespace Impacto::Profile::GameSpecific;

MenuTransitionAnimation::MenuTransitionAnimation() {
  SetDuration(MenuTransitionDuration);
}

glm::vec2 MenuTransitionAnimation::GetPageOffset() const {
  glm::vec2 position = {0.0f, 0.0f};

  if (IsIn()) return position;

  const float startProgress =
      ShowPageAnimationStartTime / MenuTransitionDuration;
  const float endProgress =
      startProgress + ShowPageAnimationDuration / MenuTransitionDuration;

  if (startProgress < Progress && Progress < endProgress) {
    const float progress =
        (Progress - startProgress) / (endProgress - startProgress);
    const float angle = progress * std::numbers::pi_v<float> * 0.5f;

    position = {0.0f, (std::sin(angle) - 1.0f) * DesignHeight};

  } else if (Progress <= startProgress) {
    position = {0.0f, -DesignHeight};
  }
  return position;
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto