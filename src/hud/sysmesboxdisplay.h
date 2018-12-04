#pragma once

namespace Impacto {
namespace SysMesBoxDisplay {

extern int MessageCount;
extern float BoxOpacity;

enum SysMesBoxAnimState { Hidden, Hiding, Showing, Shown };

extern SysMesBoxAnimState AnimState;

void Init();
void Update(float dt);
void Render();

}  // namespace SysMesBoxDisplay
}  // namespace Impacto