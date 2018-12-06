#include "../../profile_internal.h"
#include "tilebackground.h"

namespace Impacto {
namespace Profile {
namespace RNE {

Impacto::RNE::TileBackground* ParseTileBackground() {
  AssertIs(kObjectType);

  Impacto::RNE::TileBackground* result = new Impacto::RNE::TileBackground();

  result->DurationIn = EnsureGetMemberFloat("DurationIn");
  result->DurationOut = EnsureGetMemberFloat("DurationOut");

  result->Seed = EnsureGetMemberUint("Seed");

  result->Rows = EnsureGetMemberInt("Rows");
  result->Columns = EnsureGetMemberInt("Columns");

  result->VanishingPointX = EnsureGetMemberFloat("VanishingPointX");
  result->CenterY = EnsureGetMemberFloat("CenterY");
  result->Depth = EnsureGetMemberFloat("Depth");

  result->MaxAngle = EnsureGetMemberFloat("MaxAngle");

  result->BackgroundSprite = EnsureGetMemberSprite("Sprite");

  result->Init();

  return result;
}

}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto