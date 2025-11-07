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
  YNChipBlurBg0 = EnsureGetMember<Sprite>("YNChipBlurBg0");
  YNChipBlurBg1 = EnsureGetMember<Sprite>("YNChipBlurBg1");
  YNChipBlurBg2 = EnsureGetMember<Sprite>("YNChipBlurBg2");
  YN1YesChipSmall = EnsureGetMember<Sprite>("YN1ChipYesS");
  YN1YesChipLarge = EnsureGetMember<Sprite>("YN1ChipYesL");
  YN1NoChipSmall = EnsureGetMember<Sprite>("YN1ChipNoS");
  YN1NoChipLarge = EnsureGetMember<Sprite>("YN1ChipNoL");
  YN2YesChipSmall = EnsureGetMember<Sprite>("YN2ChipYesS");
  YN2YesChipLarge = EnsureGetMember<Sprite>("YN2ChipYesL");
  YN2NoChipSmall = EnsureGetMember<Sprite>("YN2ChipNoS");
  YN2NoChipLarge = EnsureGetMember<Sprite>("YN2ChipNoL");

  StarChip = EnsureGetMember<Sprite>("ChipStar");
  YesNoBlurMask = EnsureGetMember<Sprite>("YNBlurMask");
  YesNoBgOverlay = EnsureGetMember<Sprite>("YNBgOverlay");

  {
    EnsurePushMemberOfType("YesNoData1", LUA_TTABLE);

    PushInitialIndex();

    while (PushNextTableElement()) {
      AssertIs(LUA_TTABLE);
      int i = EnsureGetKey<uint32_t>() - 1;
      YesNoData1[i].BgPos = EnsureGetMember<glm::vec2>("bgPos");
      YesNoData1[i].NextYesIndex = EnsureGetMember<uint32_t>("index");
      YesNoData1[i].NextNoIndex = EnsureGetMember<uint32_t>("index2");
      YesNoData1[i].ChipYesPos = EnsureGetMember<glm::vec2>("yesChipPos");
      YesNoData1[i].ChipNoPos = EnsureGetMember<glm::vec2>("noChipPos");
      YesNoData1[i].BlurMaskPos = EnsureGetMember<glm::vec2>("bubblePos");
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
      YesNoData2[i].BgPos = EnsureGetMember<glm::vec2>("bgPos");
      YesNoData2[i].NextYesIndex = EnsureGetMember<uint32_t>("index");
      YesNoData2[i].NextNoIndex = EnsureGetMember<uint32_t>("index2");
      YesNoData2[i].ChipYesPos = EnsureGetMember<glm::vec2>("yesChipPos");
      YesNoData2[i].ChipNoPos = EnsureGetMember<glm::vec2>("noChipPos");
      YesNoData2[i].BlurMaskPos = EnsureGetMember<glm::vec2>("bubblePos");
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