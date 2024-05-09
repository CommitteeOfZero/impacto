#pragma once
#include "../../../games/cclcc/yesnotrigger.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace YesNoTrigger {

bool Configure();

const int BackgroundPositionsNum = 8;
const int YesNoDataSize = 20;

extern Sprite YesNoBackground0;
extern Sprite YesNoBackground1;
extern Sprite YesNoBackground2;
extern Sprite YesNoBackground3;
extern Sprite YN1_YesChipSmall;
extern Sprite YN1_YesChipLarge;
extern Sprite YN1_NoChipSmall;
extern Sprite YN1_NoChipLarge;
extern Sprite YN2_YesChipSmall;
extern Sprite YN2_YesChipLarge;
extern Sprite YN2_NoChipSmall;
extern Sprite YN2_NoChipLarge;
extern Sprite StarChip;
extern Sprite YesNoBgMask;

extern glm::vec2 BackgroundPositions[BackgroundPositionsNum];
extern Impacto::CCLCC::YesNoTrigger::YesNoPositions YesNoData1[YesNoDataSize];
extern Impacto::CCLCC::YesNoTrigger::YesNoPositions YesNoData2[YesNoDataSize];

}  // namespace YesNoTrigger
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto