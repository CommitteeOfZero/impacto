#include "tipsnotification.h"

#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsNotification {

void Configure() {
  HeaderMessageId = EnsureGetMember<int>("HeaderMessageId");
  HeaderPosition = EnsureGetMember<glm::vec2>("HeaderPosition");
  HeaderFontSize = EnsureGetMember<float>("HeaderFontSize");
  HeaderColor = EnsureGetMember<DialogueColorPair>("HeaderColor");

  TextStartPosition = EnsureGetMember<glm::vec2>("TextStartPosition");
  TextTargetPosition = EnsureGetMember<glm::vec2>("TextTargetPosition");
  TextFontSize = EnsureGetMember<float>("TextFontSize");
  TipNameColorIndex = EnsureGetMember<size_t>("TipNameColorIndex");

  SlideTime = EnsureGetMember<float>("SlideTime");
  HoldTime = EnsureGetMember<float>("HoldTime");

  RenderBounds = EnsureGetMember<RectF>("RenderBounds");
}

}  // namespace TipsNotification
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
