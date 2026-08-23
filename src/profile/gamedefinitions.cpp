#include "gamedefinitions.h"

#include "profile_internal.h"
namespace Impacto::Profile {
template <>
struct TryGetImpl<GameDefinition> {
  static std::optional<GameDefinition> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    auto gameProfileOpt = TryGetMember<std::string>("GameProfile");
    auto name = TryGetMember<std::string>("Name").value_or("");
    auto patch =
        TryGetMember<decltype(GameDefinition::Patch)>("Patch").value_or(
            decltype(GameDefinition::Patch){});
    auto hidden = TryGetMember<bool>("Hidden").value_or(false);
    auto launcherOrderId = TryGetMember<int>("LauncherOrderId").value_or(99);
    auto launcherTheme =
        TryGetMember<uint32_t>("LauncherTheme").value_or(0xFFFFFF);

    if (!gameProfileOpt || gameProfileOpt->empty()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Missing GameProfile path\n");
      return std::nullopt;
    }
    return GameDefinition{
        .GameProfile = std::move(*gameProfileOpt),
        .Name = std::move(name),
        .Patch = std::move(patch),
        .Hidden = hidden,
        .LauncherOrderId = launcherOrderId,
        .LauncherTheme = launcherTheme,
    };
  }
};

void GameDefinition::Configure() {
  GameDefinitions =
      EnsureGetMember<decltype(GameDefinitions)>("GameDefinitions");
}

}  // namespace Impacto::Profile