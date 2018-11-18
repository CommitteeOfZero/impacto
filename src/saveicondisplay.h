#pragma once

namespace Impacto {
namespace SaveIconDisplay {

extern float PositionX;
extern float PositionY;

enum SaveIconAnimState { Hidden, Hiding, Showing, Shown };

extern SaveIconAnimState AnimState;

void Update(float dt);
void Render();
}  // namespace SaveIconDisplay
}  // namespace Impacto