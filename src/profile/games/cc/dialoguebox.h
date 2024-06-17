#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace DialogueBox {

void Configure();

int constexpr NamePlateCountMax = 80;

inline Sprite ADVBoxMask;
inline Sprite NamePlateMainSprites[NamePlateCountMax];
inline Sprite NamePlateLabelSprites[NamePlateCountMax];

inline glm::vec2 ADVBoxNamePlateMainPos;
inline glm::vec2 ADVBoxNamePlateLabelPos;

inline float ADVBoxEffectDuration;

}  // namespace DialogueBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto