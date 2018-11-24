#include "game.h"
#include "profile.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {

int LayerCount;
int GameFeatures;

float DesignWidth;
float DesignHeight;

void LoadFromJson() {
  AssertIs(Json, "/", kObjectType);
  AssertHasMember(Json, "/", "LayerCount");
  AssertHasMember(Json, "/", "GameFeatures");
  AssertHasMember(Json, "/", "DesignWidth");
  AssertHasMember(Json, "/", "DesignHeight");

  auto const& _layerCount = Json["LayerCount"];
  auto const& _gameFeatures = Json["GameFeatures"];
  auto const& _designWidth = Json["DesignWidth"];
  auto const& _designHeight = Json["DesignHeight"];

  AssertIsIntegral(_layerCount, "/LayerCount");
  AssertIsIntegral(_gameFeatures, "/GameFeatures");
  AssertIs(_designWidth, "/DesignWidth", kNumberType);
  AssertIs(_designWidth, "/DesignHeight", kNumberType);

  LayerCount = _layerCount.GetInt();
  GameFeatures = _gameFeatures.GetInt();
  DesignWidth = _designWidth.GetFloat();
  DesignHeight = _designHeight.GetFloat();
}

}  // namespace Profile
}  // namespace Impacto