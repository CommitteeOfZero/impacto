#include "achievementnotification.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace AchievementNotification {

void Configure() {
  EnsurePushMemberOfType("AchievementNotification", LUA_TTABLE);

  BackgroundPath = EnsureGetMember<std::string>("BackgroundPath");
  FontPath = EnsureGetMember<std::string>("FontPath");
  DisplayDuration = EnsureGetMember<float>("DisplayDuration");
  FadeDuration = EnsureGetMember<float>("FadeDuration");
  IconSize = EnsureGetMember<float>("IconSize");
  IconOffset = EnsureGetMember<glm::vec2>("IconOffset");
  TextGap = EnsureGetMember<float>("TextGap");
  TextRightPadding = EnsureGetMember<float>("TextRightPadding");
  TitleFontSize = EnsureGetMember<float>("TitleFontSize");
  DescriptionFontSize = EnsureGetMember<float>("DescriptionFontSize");
  TextLineGap = EnsureGetMember<float>("TextLineGap");
  TextColor = EnsureGetMember<uint32_t>("TextColor");
  OutlineColor = EnsureGetMember<uint32_t>("OutlineColor");

  Pop();
}

}  // namespace AchievementNotification
}  // namespace Profile
}  // namespace Impacto
