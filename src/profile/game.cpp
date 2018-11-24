#include "game.h"
#include "profile.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {

int LayerCount;
int GameFeatures;

float DesignWidth;
float DesignHeight;

void LoadGameFromJson() {
  AssertIs(Json, "/", kObjectType);

  LayerCount = EnsureGetMemberInt(Json, "/", "LayerCount");
  GameFeatures = EnsureGetMemberInt(Json, "/", "GameFeatures");
  DesignWidth = EnsureGetMemberFloat(Json, "/", "DesignWidth");
  DesignHeight = EnsureGetMemberFloat(Json, "/", "DesignHeight");
}

}  // namespace Profile
}  // namespace Impacto