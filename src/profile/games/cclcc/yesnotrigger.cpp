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

  YesNoBackground0 = EnsureGetMember<Sprite>("YesNoBackground0");
  YesNoBackground1 = EnsureGetMember<Sprite>("YesNoBackground1");
  YesNoBackground2 = EnsureGetMember<Sprite>("YesNoBackground2");
  YesNoBackground3 = EnsureGetMember<Sprite>("YesNoBackground3");
  YN1YesChipSmall = EnsureGetMember<Sprite>("YN1ChipYesS");
  YN1YesChipLarge = EnsureGetMember<Sprite>("YN1ChipYesL");
  YN1NoChipSmall = EnsureGetMember<Sprite>("YN1ChipNoS");
  YN1NoChipLarge = EnsureGetMember<Sprite>("YN1ChipNoL");
  YN2YesChipSmall = EnsureGetMember<Sprite>("YN2ChipYesS");
  YN2YesChipLarge = EnsureGetMember<Sprite>("YN2ChipYesL");
  YN2NoChipSmall = EnsureGetMember<Sprite>("YN2ChipNoS");
  YN2NoChipLarge = EnsureGetMember<Sprite>("YN2ChipNoL");

  StarChip = EnsureGetMember<Sprite>("ChipStar");
  YesNoBgMask = EnsureGetMember<Sprite>("BGMask");

  {
    EnsurePushMemberOfType("YesNoData1", LUA_TTABLE);

    PushInitialIndex();

    while (PushNextTableElement()) {
      AssertIs(LUA_TTABLE);
      int i = EnsureGetKey<uint32_t>() - 1;
      YesNoData1[i].BgPos.x = EnsureGetMember<float>("bgXpos");
      YesNoData1[i].BgPos.y = EnsureGetMember<float>("bgYpos");
      YesNoData1[i].NextYesIndex = EnsureGetMember<uint32_t>("index");
      YesNoData1[i].NextNoIndex = EnsureGetMember<uint32_t>("index2");
      YesNoData1[i].ChipYesPos.x = EnsureGetMember<float>("yesChipX");
      YesNoData1[i].ChipYesPos.y = EnsureGetMember<float>("yesChipY");
      YesNoData1[i].ChipNoPos.x = EnsureGetMember<float>("noChipX");
      YesNoData1[i].ChipNoPos.y = EnsureGetMember<float>("noChipY");
      YesNoData1[i].BubblePos.x = EnsureGetMember<float>("bubbleX");
      YesNoData1[i].BubblePos.y = EnsureGetMember<float>("bubbleY");
      Pop();
    }

    Pop();
  }

  {
    EnsurePushMemberOfType("YesNoData2", LUA_TTABLE);

    PushInitialIndex();

    while (PushNextTableElement()) {
      AssertIs(LUA_TTABLE);
      int i = EnsureGetKey<uint32_t>() - 1;
      YesNoData2[i].BgPos.x = EnsureGetMember<float>("bgXpos");
      YesNoData2[i].BgPos.y = EnsureGetMember<float>("bgYpos");
      YesNoData2[i].NextYesIndex = EnsureGetMember<uint32_t>("index");
      YesNoData2[i].NextNoIndex = EnsureGetMember<uint32_t>("index2");
      YesNoData2[i].ChipYesPos.x = EnsureGetMember<float>("yesChipX");
      YesNoData2[i].ChipYesPos.y = EnsureGetMember<float>("yesChipY");
      YesNoData2[i].ChipNoPos.x = EnsureGetMember<float>("noChipX");
      YesNoData2[i].ChipNoPos.y = EnsureGetMember<float>("noChipY");
      YesNoData1[i].BubblePos.x = EnsureGetMember<float>("bubbleX");
      YesNoData1[i].BubblePos.y = EnsureGetMember<float>("bubbleY");
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