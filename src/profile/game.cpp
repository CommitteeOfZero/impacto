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
  AssertIs(kObjectType);

  LayerCount = EnsureGetMemberInt("LayerCount");
  GameFeatures = EnsureGetMemberInt("GameFeatures");
  DesignWidth = EnsureGetMemberFloat("DesignWidth");
  DesignHeight = EnsureGetMemberFloat("DesignHeight");
}

}  // namespace Profile
}  // namespace Impacto