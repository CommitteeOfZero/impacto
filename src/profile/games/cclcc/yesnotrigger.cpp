#include "yesnotrigger.h"
#include "../../../log.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"

using namespace Impacto::CCLCC::YesNoTrigger;
namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace YesNoTrigger {

Sprite YesNoBackground0;
Sprite YesNoBackground1;
Sprite YesNoBackground2;
Sprite YesNoBackground3;
Sprite YN1_YesChipSmall;
Sprite YN1_YesChipLarge;
Sprite YN1_NoChipSmall;
Sprite YN1_NoChipLarge;
Sprite YN2_YesChipSmall;
Sprite YN2_YesChipLarge;
Sprite YN2_NoChipSmall;
Sprite YN2_NoChipLarge;
Sprite StarChip;
Sprite YesNoBgMask;

glm::vec2 BackgroundPositions[BackgroundPositionsNum];

YesNoPositions YesNoData1[YesNoDataSize];
YesNoPositions YesNoData2[YesNoDataSize];

bool Configure() {
  if (!TryPushMember("YesNoTrigger")) return false;
  AssertIs(LUA_TTABLE);

  YesNoBackground0 = EnsureGetMemberSprite("YesNoBackground0");
  YesNoBackground1 = EnsureGetMemberSprite("YesNoBackground1");
  YesNoBackground2 = EnsureGetMemberSprite("YesNoBackground2");
  YesNoBackground3 = EnsureGetMemberSprite("YesNoBackground3");
  YN1_YesChipSmall = EnsureGetMemberSprite("YN1_ChipYesS");
  YN1_YesChipLarge = EnsureGetMemberSprite("YN1_ChipYesL");
  YN1_NoChipSmall = EnsureGetMemberSprite("YN1_ChipNoS");
  YN1_NoChipLarge = EnsureGetMemberSprite("YN1_ChipNoL");
  YN2_YesChipSmall = EnsureGetMemberSprite("YN2_ChipYesS");
  YN2_YesChipLarge = EnsureGetMemberSprite("YN2_ChipYesL");
  YN2_NoChipSmall = EnsureGetMemberSprite("YN2_ChipNoS");
  YN2_NoChipLarge = EnsureGetMemberSprite("YN2_ChipNoL");

  StarChip = EnsureGetMemberSprite("ChipStar");
  YesNoBgMask = EnsureGetMemberSprite("BGMask");

  {
    EnsurePushMemberOfType("YesNoData1", LUA_TTABLE);

    PushInitialIndex();

    while (PushNextTableElement()) {
      AssertIs(LUA_TTABLE);
      int i = EnsureGetKeyUint() - 1;
      YesNoData1[i].bgXPos = EnsureGetMemberFloat("bgXpos");
      YesNoData1[i].bgYPos = EnsureGetMemberFloat("bgYpos");
      YesNoData1[i].nextYesIndex = EnsureGetMemberUint("index");
      YesNoData1[i].nextNoIndex = EnsureGetMemberUint("index2");
      YesNoData1[i].chipYesXPos = EnsureGetMemberFloat("yesChipX");
      YesNoData1[i].chipYesYPos = EnsureGetMemberFloat("yesChipY");
      YesNoData1[i].chipNoXPos = EnsureGetMemberFloat("noChipX");
      YesNoData1[i].chipNoYPos = EnsureGetMemberFloat("noChipY");
      YesNoData1[i].bubbleXPos = EnsureGetMemberFloat("bubbleX");
      YesNoData1[i].bubbleYPos = EnsureGetMemberFloat("bubbleY");
      Pop();
    }

    Pop();
  }

  {
    EnsurePushMemberOfType("YesNoData2", LUA_TTABLE);

    PushInitialIndex();

    while (PushNextTableElement()) {
      AssertIs(LUA_TTABLE);
      int i = EnsureGetKeyUint() - 1;
      YesNoData2[i].bgXPos = EnsureGetMemberFloat("bgXpos");
      YesNoData2[i].bgYPos = EnsureGetMemberFloat("bgYpos");
      YesNoData2[i].nextYesIndex = EnsureGetMemberUint("index");
      YesNoData2[i].nextNoIndex = EnsureGetMemberUint("index2");
      YesNoData2[i].chipYesXPos = EnsureGetMemberFloat("yesChipX");
      YesNoData2[i].chipYesYPos = EnsureGetMemberFloat("yesChipY");
      YesNoData2[i].chipNoXPos = EnsureGetMemberFloat("noChipX");
      YesNoData2[i].chipNoYPos = EnsureGetMemberFloat("noChipY");
      YesNoData2[i].bubbleXPos = EnsureGetMemberFloat("bubbleX");
      YesNoData2[i].bubbleYPos = EnsureGetMemberFloat("bubbleY");
      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace YesNoTrigger
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto