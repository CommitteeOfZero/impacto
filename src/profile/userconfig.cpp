#include "userconfig.h"

#include <optional>
#include "profile_internal.h"

namespace Impacto::Profile {

template <>
struct TryGetImpl<UserConfig::GameConfig> {
  static std::optional<UserConfig::GameConfig> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    return UserConfig::GameConfig{
        .Language = TryGetMember<std::string>("Language").value_or("Japanese"),
        .UsePatch = TryGetMember<bool>("UsePatch").value_or(false),
    };
  }
};

template <>
struct TryGetImpl<UserConfig::Config> {
  static std::optional<UserConfig::Config> Call() {
    if (!lua_istable(LuaState, -1)) return std::nullopt;

    auto tryLog = []<typename T>(char const* name) {
      auto opt = TryGetMember<T>(name);
      if (!opt) {
        ImpLog(LogLevel::Error, LogChannel::Profile,
               "Missing {:s} in profile\n", name);
      }
      return opt;
    };

    auto resolutionWidthOpt = tryLog.operator()<int>("ResolutionWidth");
    auto resolutionHeightOpt = tryLog.operator()<int>("ResolutionHeight");
    auto useNativeGameResolution =
        TryGetMember<bool>("UseNativeGameResolution").value_or(false);
    auto fullscreen = TryGetMember<bool>("Fullscreen").value_or(false);
    auto subtitleConfig =
        TryGetMember<Subtitle::SubtitleConfigType>("SubtitleConfig")
            .value_or(Subtitle::SubtitleConfigType::None);
    auto closeBacklogWhenReachedEnd =
        TryGetMember<bool>("CloseBacklogWhenReachedEnd").value_or(true);

    if (!resolutionWidthOpt || !resolutionHeightOpt) return std::nullopt;
    return UserConfig::Config{
        .ResolutionWidth = *resolutionWidthOpt,
        .ResolutionHeight = *resolutionHeightOpt,
        .UseNativeGameResolution = useNativeGameResolution,
        .Fullscreen = fullscreen,
        .SubtitleConfig = subtitleConfig,
        .CloseBacklogWhenReachedEnd = closeBacklogWhenReachedEnd};
  }
};
namespace UserConfig {
void Configure() {
  EnsurePushMemberOfType("UserConfig", LUA_TTABLE);
  GameSettings = EnsureGetMember<decltype(GameSettings)>("GameSettings");
  CommonSettings = EnsureGetMember<Config>("CommonSettings");
  if (ActiveGame.empty())
    ActiveGame = EnsureGetMember<std::string>("ActiveGame");
  auto gameSettingsItr = GameSettings.find(ActiveGame);
  if (gameSettingsItr == GameSettings.end()) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "ActiveGame {:s} not found in Games\n", ActiveGame);
    exit(1);
  }
};

GameConfig const& ActiveGameSettings() { return GameSettings.at(ActiveGame); }

std::string const& GetPlatformSpecificPath() {
  static const std::string result = [] {
    std::filesystem::path configDir = Io::GetPlatformConfigDir();
    return (configDir / "userconfig.lua").string();
  }();
  return result;
}

}  // namespace UserConfig
}  // namespace Impacto::Profile