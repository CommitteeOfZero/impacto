#pragma once

#include "../sprites.h"
#include "../../hud/delusiontrigger.h"

namespace Impacto {
namespace Profile {
namespace DelusionTrigger {

extern Sprite BackgroundSprite;
extern Sprite BackgroundSpriteMask;
extern Sprite ScreenMask;

void Configure();

void CreateInstance();

}  // namespace TipsNotification
}  // namespace Profile
}  // namespace Impacto