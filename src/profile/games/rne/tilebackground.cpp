#include "../../profile_internal.h"
#include "tilebackground.h"

namespace Impacto {
namespace Profile {
namespace RNE {

Impacto::RNE::TileBackground* ParseTileBackground() {
  AssertIs(LUA_TTABLE);

  Impacto::RNE::TileBackground* result = new Impacto::RNE::TileBackground();

  result->DurationIn = EnsureGetMember<float>("DurationIn");
  result->DurationOut = EnsureGetMember<float>("DurationOut");

  result->Seed = EnsureGetMember<uint32_t>("Seed");

  result->Rows = EnsureGetMember<int>("Rows");
  result->Columns = EnsureGetMember<int>("Columns");

  result->VanishingPointX = EnsureGetMember<float>("VanishingPointX");
  result->CenterY = EnsureGetMember<float>("CenterY");
  result->Depth = EnsureGetMember<float>("Depth");

  result->MaxAngle = EnsureGetMember<float>("MaxAngle");

  result->BackgroundSprite = EnsureGetMember<Sprite>("Sprite");

  result->Init();

  return result;
}

}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto