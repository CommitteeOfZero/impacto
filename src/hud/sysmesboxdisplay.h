#pragma once

#include "../text.h"

namespace Impacto {
namespace SysMesBoxDisplay {

extern int MessageCount;
extern float BoxOpacity;

enum SysMesBoxAnimState { Hidden, Hiding, Showing, Shown };

extern SysMesBoxAnimState AnimState;

extern ProcessedTextGlyph Messages[8][255];

void Init();
void Update(float dt);
void Render();

}  // namespace SysMesBoxDisplay
}  // namespace Impacto