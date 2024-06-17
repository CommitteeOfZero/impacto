#pragma once

#include "../sprites.h"
#include "../../hud/delusiontrigger.h"

namespace Impacto {
namespace Profile {
namespace DelusionTrigger {
inline Impacto::DelusionTrigger::DelusionTriggerType Type =
    Impacto::DelusionTrigger::DelusionTriggerType::None;

void Configure();

void CreateInstance();

}  // namespace DelusionTrigger
}  // namespace Profile
}  // namespace Impacto