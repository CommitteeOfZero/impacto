#include "baseconfig.h"

#include "profile_internal.h"
#include "../io/filemeta.h"
namespace Impacto::Profile {
template <>
struct TryGetImpl<BaseConfig::GameDefinition> {
  static std::optional<BaseConfig::GameDefinition> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    auto gameProfileOpt = TryGetMember<std::string>("GameProfile");
    auto patch =
        TryGetMember<decltype(BaseConfig::GameDefinition::Patch)>("Patch")
            .value_or(decltype(BaseConfig::GameDefinition::Patch){});

    if (!gameProfileOpt || gameProfileOpt->empty()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Missing GameProfile path\n");
      return std::nullopt;
    }
    return BaseConfig::GameDefinition{
        .GameProfile = std::move(*gameProfileOpt),
        .Patch = std::move(patch),
    };
  }
};

namespace BaseConfig {

void Configure() {
  EnsurePushMemberOfType("BaseConfig", LUA_TTABLE);
  GameDefinitions =
      EnsureGetMember<decltype(GameDefinitions)>("GameDefinitions");

  RootGamedataDir = EnsureGetMember<std::string>("RootGamedataDir");
  RootProfilesDir = EnsureGetMember<std::string>("RootProfilesDir");
  RootPatchesDir = TryGetMember<std::string>("RootPatchesDir").value_or("");
  RootSavesDir = EnsureGetMember<std::string>("RootSavesDir");
  Pop();
}

std::string const& GetPlatformSpecificPath() {
  static const std::string result = [] {
    std::filesystem::path configDir = Io::GetPlatformConfigDir();
    return (configDir / "baseconfig.lua").string();
  }();
  return result;
}
}  // namespace BaseConfig
}  // namespace Impacto::Profile