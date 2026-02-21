#pragma once

namespace Impacto {
namespace AchievementNotification {

inline bool IsShowing = false;

void Init();
void Update(float dt);
void Render();
void Show(int achievementId);

}  // namespace AchievementNotification
}  // namespace Impacto
