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
extern Sprite YN1YesChipSmall;
extern Sprite YN1YesChipLarge;
extern Sprite YN1NoChipSmall;
extern Sprite YN1NoChipLarge;
extern Sprite YN2YesChipSmall;
extern Sprite YN2YesChipLarge;
extern Sprite YN2NoChipSmall;
extern Sprite YN2NoChipLarge;
extern Sprite StarChip;
extern Sprite YesNoBgMask;

extern glm::vec2 BackgroundPositions[BackgroundPositionsNum];
extern Impacto::CCLCC::YesNoTrigger::YesNoPositions YesNoData1[YesNoDataSize];
extern Impacto::CCLCC::YesNoTrigger::YesNoPositions YesNoData2[YesNoDataSize];

}  // namespace YesNoTrigger
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto