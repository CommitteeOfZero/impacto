#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace DialogueBox {

void Configure();
void ConfigureNametag();

inline Sprite ADVBoxMask;

inline float ADVBoxEffectDuration;

inline std::vector<Sprite> NametagMainSprites;
inline std::vector<Sprite> NametagLabelSprites;

inline glm::vec2 NametagMainPos = glm::vec2(0.0f);
inline glm::vec2 NametagLabelPos = glm::vec2(0.0f);

inline float NametagShowDuration = 0.0f;

}  // namespace DialogueBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto