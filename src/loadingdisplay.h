#pragma once

namespace Impacto {
namespace LoadingDisplay {

enum LoadingAnimState { Hidden, Hiding, Showing, Shown };

extern LoadingAnimState AnimState;

void Update(float dt);
void Render();
}  // namespace LoadingDisplay
}  // namespace Impacto