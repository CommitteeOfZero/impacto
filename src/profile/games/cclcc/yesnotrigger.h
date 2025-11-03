#pragma once
#include "../../../games/cclcc/yesnotrigger.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace YesNoTrigger {

bool Configure();

constexpr int BackgroundPositionsNum = 8;
constexpr int YesNoDataSize = 20;

inline Sprite YesNoBackground0;
inline Sprite YesNoBackground1;
inline Sprite YesNoBackground2;
inline Sprite YesNoBackground3;
inline Sprite YN1YesChipSmall;
inline Sprite YN1YesChipLarge;
inline Sprite YN1NoChipSmall;
inline Sprite YN1NoChipLarge;
inline Sprite YN2YesChipSmall;
inline Sprite YN2YesChipLarge;
inline Sprite YN2NoChipSmall;
inline Sprite YN2NoChipLarge;
inline Sprite StarChip;
inline Sprite YesNoBgMask;

inline glm::vec2 BackgroundPositions[BackgroundPositionsNum];
inline UI::CCLCC::YesNoTrigger::YesNoPositions YesNoData1[YesNoDataSize];
inline UI::CCLCC::YesNoTrigger::YesNoPositions YesNoData2[YesNoDataSize];

}  // namespace YesNoTrigger
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto