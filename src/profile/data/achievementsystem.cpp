#include "achievementsystem.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {

template <>
struct TryGetImpl<AchievementSystem::AchievementDef> {
  static std::optional<AchievementSystem::AchievementDef> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;
    auto nameOpt = TryGetMember<std::string>("Name");
    auto descriptionOpt = TryGetMember<std::string>("Description");
    auto rarityOpt =
        TryGetMember<Impacto::AchievementSystem::AchievementRarity>("Rarity");
    auto iconPathOpt = TryGetMember<std::string>("IconPath");
    bool hidden = TryGetMember<bool>("Hidden").value_or(false);

    if (!nameOpt || !descriptionOpt || !rarityOpt || !iconPathOpt)
      return std::nullopt;

    return AchievementSystem::AchievementDef{
        .Name = std::move(*nameOpt),
        .Description = std::move(*descriptionOpt),
        .Hidden = hidden,
        .Rarity = *rarityOpt,
        .IconPath = std::move(*iconPathOpt),
    };
  }
};

}  // namespace Profile

namespace Profile {
namespace AchievementSystem {

using namespace Impacto::AchievementSystem;

void Configure() {
  if (TryPushMember("AchievementData")) {
    AssertIs(LUA_TTABLE);
    Type = EnsureGetMember<AchievementDataType>("Type");

    switch (Type) {
      case AchievementDataType::Common:
        Achievements =
            EnsureGetMember<std::vector<AchievementDef>>("Achievements");
        AchievementDataPath = EnsureGetMember<std::string>("AchievementSystem");
        Implementation = new AchievementSystemCommon();
        break;

      case AchievementDataType::None:
        break;
    }

    Pop();
  }
}
}  // namespace AchievementSystem
}  // namespace Profile
}  // namespace Impacto
