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

  LayerCount =
      EnsureGetInt(EnsureGetMember(Json, "/", "LayerCount"), "/LayerCount");
  GameFeatures =
      EnsureGetInt(EnsureGetMember(Json, "/", "GameFeatures"), "/GameFeatures");
  DesignWidth =
      EnsureGetFloat(EnsureGetMember(Json, "/", "DesignWidth"), "/DesignWidth");
  DesignHeight = EnsureGetFloat(EnsureGetMember(Json, "/", "DesignHeight"),
                                "/DesignHeight");
}

}  // namespace Profile
}  // namespace Impacto