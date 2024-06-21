#include "yesnotrigger.h"
#include "../../../log.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace YesNoTrigger {

bool Configure() {
  if (!TryPushMember("YesNoTrigger")) return false;
  AssertIs(LUA_TTABLE);

  YesNoBackground0 = EnsureGetMemberSprite("YesNoBackground0");
  YesNoBackground1 = EnsureGetMemberSprite("YesNoBackground1");
  YesNoBackground2 = EnsureGetMemberSprite("YesNoBackground2");
  YesNoBackground3 = EnsureGetMemberSprite("YesNoBackground3");
  YN1YesChipSmall = EnsureGetMemberSprite("YN1ChipYesS");
  YN1YesChipLarge = EnsureGetMemberSprite("YN1ChipYesL");
  YN1NoChipSmall = EnsureGetMemberSprite("YN1ChipNoS");
  YN1NoChipLarge = EnsureGetMemberSprite("YN1ChipNoL");
  YN2YesChipSmall = EnsureGetMemberSprite("YN2ChipYesS");
  YN2YesChipLarge = EnsureGetMemberSprite("YN2ChipYesL");
  YN2NoChipSmall = EnsureGetMemberSprite("YN2ChipNoS");
  YN2NoChipLarge = EnsureGetMemberSprite("YN2ChipNoL");

  StarChip = EnsureGetMemberSprite("ChipStar");
  YesNoBgMask = EnsureGetMemberSprite("BGMask");

  {
    EnsurePushMemberOfType("YesNoData1", LUA_TTABLE);

    PushInitialIndex();

    while (PushNextTableElement()) {
      AssertIs(LUA_TTABLE);
      int i = EnsureGetKeyUint() - 1;
      YesNoData1[i].BgPos.x = EnsureGetMemberFloat("bgXpos");
      YesNoData1[i].BgPos.y = EnsureGetMemberFloat("bgYpos");
      YesNoData1[i].NextYesIndex = EnsureGetMemberUint("index");
      YesNoData1[i].NextNoIndex = EnsureGetMemberUint("index2");
      YesNoData1[i].ChipYesPos.x = EnsureGetMemberFloat("yesChipX");
      YesNoData1[i].ChipYesPos.y = EnsureGetMemberFloat("yesChipY");
      YesNoData1[i].ChipNoPos.x = EnsureGetMemberFloat("noChipX");
      YesNoData1[i].ChipNoPos.y = EnsureGetMemberFloat("noChipY");
      YesNoData1[i].BubblePos.x = EnsureGetMemberFloat("bubbleX");
      YesNoData1[i].BubblePos.y = EnsureGetMemberFloat("bubbleY");
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
      YesNoData2[i].BgPos.x = EnsureGetMemberFloat("bgXpos");
      YesNoData2[i].BgPos.y = EnsureGetMemberFloat("bgYpos");
      YesNoData2[i].NextYesIndex = EnsureGetMemberUint("index");
      YesNoData2[i].NextNoIndex = EnsureGetMemberUint("index2");
      YesNoData2[i].ChipYesPos.x = EnsureGetMemberFloat("yesChipX");
      YesNoData2[i].ChipYesPos.y = EnsureGetMemberFloat("yesChipY");
      YesNoData2[i].ChipNoPos.x = EnsureGetMemberFloat("noChipX");
      YesNoData2[i].ChipNoPos.y = EnsureGetMemberFloat("noChipY");
      YesNoData1[i].BubblePos.x = EnsureGetMemberFloat("bubbleX");
      YesNoData1[i].BubblePos.y = EnsureGetMemberFloat("bubbleY");
      Pop();
    }

    Pop();
  }

  Pop();
  return true;
}

}  // namespace YesNoTrigger
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto