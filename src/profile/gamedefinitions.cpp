#include "gamedefinitions.h"

#include "profile_internal.h"
#include "../io/filemeta.h"
namespace Impacto::Profile {
template <>
struct TryGetImpl<GameDefinition> {
  static std::optional<GameDefinition> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    auto gameProfileOpt = TryGetMember<std::string>("GameProfile");
    auto patch =
        TryGetMember<decltype(GameDefinition::Patch)>("Patch").value_or(
            decltype(GameDefinition::Patch){});

    if (!gameProfileOpt || gameProfileOpt->empty()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Missing GameProfile path\n");
      return std::nullopt;
    }
    return GameDefinition{
        .GameProfile = std::move(*gameProfileOpt),
        .Patch = std::move(patch),
    };
  }
};

void GameDefinition::Configure() {
  GameDefinitions =
      EnsureGetMember<decltype(GameDefinitions)>("GameDefinitions");
}

}  // namespace Impacto::Profile