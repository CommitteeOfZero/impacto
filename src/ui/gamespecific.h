#pragma once

#include "../games/cclcc/delusiontrigger.h"
#include "../games/chlcc/delusiontrigger.h"
#include "../games/cclcc/yesnotrigger.h"
#include "../games/cclcc/mapsystem.h"

namespace Impacto {
namespace UI {
namespace GameSpecific {
void Init();
void Update(float dt);
void RenderLayer(uint32_t layer);
void RenderCCButtonGuide();
void UpdateCCButtonGuide(float dt);

}  // namespace GameSpecific
}  // namespace UI
}  // namespace Impacto