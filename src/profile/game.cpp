#include "game.h"
#include "profile.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {

int LayerCount;
int GameFeatures;

char const* WindowName;

bool LayFileBigEndian;
bool CharaIsMvl;
bool UseScreenCapEffects;
float LayFileTexXMultiplier;
float LayFileTexYMultiplier;

float DesignWidth;
float DesignHeight;

void LoadGameFromJson() {
  AssertIs(kObjectType);

  LayerCount = EnsureGetMemberInt("LayerCount");
  GameFeatures = EnsureGetMemberInt("GameFeatures");
  WindowName = EnsureGetMemberString("WindowName");
  DesignWidth = EnsureGetMemberFloat("DesignWidth");
  DesignHeight = EnsureGetMemberFloat("DesignHeight");

  bool res = TryGetMemberBool("LayFileBigEndian", LayFileBigEndian);
  if (!res) LayFileBigEndian = false;
  res = TryGetMemberBool("CharaIsMvl", CharaIsMvl);
  if (!res) CharaIsMvl = false;
  res = TryGetMemberFloat("LayFileTexXMultiplier", LayFileTexXMultiplier);
  if (!res) LayFileTexXMultiplier = 1.0f;
  res = TryGetMemberFloat("LayFileTexYMultiplier", LayFileTexYMultiplier);
  if (!res) LayFileTexYMultiplier = 1.0f;
  res = TryGetMemberBool("UseScreenCapEffects", UseScreenCapEffects);
  if (!res) UseScreenCapEffects = true;
}

}  // namespace Profile
}  // namespace Impacto