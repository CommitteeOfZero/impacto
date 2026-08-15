#include "userconfig.h"

#include <optional>

#include <fmt/ranges.h>
#include <toml.hpp>
#include "io/physicalfilestream.h"
#include "profile/gamedefinitions.h"
#include "log.h"
#include "util.h"

namespace toml {
using namespace Impacto::UserConfig;

template <typename T>
  requires std::is_enum_v<T>
struct from<T> {
  static T from_toml(const toml::value& v) {
    std::optional<T> enumOpt;
    auto enumStr = toml::get<std::string_view>(v);

    std::string errStr;
    if constexpr (magic_enum::is_flags_v<T>) {
      enumOpt = magic_enum::enum_flags_cast<T>(enumStr);
      if (!enumOpt)
        errStr = fmt::format(
            "Expected a combination of following flags (join with '|'): {}",
            magic_enum::enum_values<T>());
    } else {
      enumOpt = magic_enum::enum_cast<T>(enumStr);
      if (!enumOpt)
        errStr =
            fmt::format("Expected one of {}", magic_enum::enum_values<T>());
    }

    if (!errStr.empty()) {
      auto err = toml::make_error_info(errStr, v, "but got invalid value");
      throw toml::type_error(toml::format_error("", err), v.location());
    }

    return *enumOpt;
  }
};

template <typename T>
  requires std::is_enum_v<T>
struct into<T> {
  template <typename TC>
  static toml::basic_value<TC> into_toml(const T& v) {
    using value_type = toml::basic_value<TC>;
    using string_type = typename value_type::string_type;

    std::string enumStr;
    if constexpr (magic_enum::is_flags_v<T>)
      enumStr = magic_enum::enum_flags_name<T>(v);
    else
      enumStr = magic_enum::enum_name<T>(v);

    return value_type(string_type{enumStr});
  }
};

}  // namespace toml
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(Impacto::UserConfig::CHLCCExtraConfig,
                                       DelusionMousePatch)
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(Impacto::UserConfig::CCLCCExtraConfig,
                                       DelusionMousePatch)

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(Impacto::UserConfig::GameConfig,
                                       ResolutionWidth, ResolutionHeight,
                                       PatchProfile, UsePatch, Fullscreen)
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(Impacto::UserConfig::Config,
                                       ResolutionWidth, ResolutionHeight,
                                       SubtitleConfig,
                                       CloseBacklogWhenReachedEnd)
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(Impacto::UserConfig::AdvancedConfig,
                                       ActiveRenderer, VideoPlayer,
                                       ActiveAudioBackend, SubtitleAssBackend,
                                       SubtitleTextBackend, SubtitleBmpBackend)

namespace Impacto::UserConfig {

static std::string ActiveGame;

void LoadUserConfig(toml::value& tomlConfig);

static std::unique_ptr<Io::PhysicalFileStream> openTomlFile(
    Io::PhysicalFileStream::CreateFlags flags) {
  std::unique_ptr<Io::PhysicalFileStream> userConfigFile;
  Io::Stream* stream;
  const IoError fileOpenRes =
      Io::PhysicalFileStream::Create(UserConfigPath, &stream, flags);
  if (fileOpenRes != IoError_OK) {
    throw std::runtime_error("Failed to open user configuration file.");
  }
  userConfigFile.reset(static_cast<Io::PhysicalFileStream*>(stream));
  return userConfigFile;
}

static GameExtraConfig MakeDefaultExtra(std::string const& gameId) {
  if (gameId == "chlcc") return CHLCCExtraConfig{};
  if (gameId == "cclcc") return CCLCCExtraConfig{};
  return std::monostate{};
}

static toml::value SerializeGameConfig(std::string const& gameId,
                                       GameConfig const& cfg) {
  toml::value v = cfg;

  std::visit(
      [&]<typename T>(T const& e) {
        if constexpr (!std::is_same_v<T, std::monostate>) {
          v["Extra"] = e;
        }
      },
      cfg.Extra);

  return v;
}

static GameConfig DeserializeGameConfig(std::string const& gameId,
                                        toml::value const& v) {
  GameConfig cfg = toml::get<GameConfig>(v);

  GameExtraConfig extra = MakeDefaultExtra(gameId);
  if (v.contains("Extra")) {
    std::visit(
        [&]<typename T>(T& e) {
          if constexpr (!std::is_same_v<T, std::monostate>) {
            e = toml::find<T>(v, "Extra");
          }
        },
        extra);
  }
  cfg.Extra = extra;

  return cfg;
}

void Configure() {
  constexpr auto fileName = "userconfig.toml";
  if (UserConfigPath.empty()) {
    std::filesystem::path configDir = Io::GetPlatformConfigDir();
    UserConfigPath = (configDir / fileName).string();
  }
  const IoError existsResult = Io::PathExists(UserConfigPath);
  if (existsResult == IoError_Fail) {
    throw std::runtime_error("Failed to check user configuration file.");
  }

  const bool fileMissing = existsResult == IoError_NotFound;
  if (fileMissing) {
    ImpLog(LogLevel::Info, LogChannel::Profile,
           "Configuration file \"{}\" doesn't exist, creating now.\n",
           UserConfigPath);
  }

  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  auto userConfigFile = openTomlFile(CF::READ | CF::CREATE);
  std::string fileContents(userConfigFile->Meta.Size, '\0');
  userConfigFile->Read(fileContents.data(), userConfigFile->Meta.Size);

  auto parseResult = toml::try_parse_str(fileContents, toml::spec::v(1, 1, 0));
  if (parseResult.is_err()) {
    ImpLog(LogLevel::Fatal, LogChannel::Config,
           "Error parsing user config: \n");
    for (const auto& err : parseResult.as_err()) {
      ImpLog(LogLevel::Fatal, LogChannel::Config, "\t{}\n",
             toml::format_error(err));
    }
    throw std::runtime_error("Failed to parse user configuration file.");
  }
  auto&& tomlConfig = parseResult.as_ok();

  LoadUserConfig(tomlConfig);

  WriteUserConfig();
}

void LoadUserConfig(toml::value& tomlConfig) {
  if (auto commonSettingsOpt =
          toml::find<std::optional<decltype(CommonSettings)>>(
              tomlConfig, "CommonSettings")) {
    CommonSettings = std::move(*commonSettingsOpt);
  }
  if (auto gameSettingsOpt =
          toml::find<std::optional<toml::table>>(tomlConfig, "GameSettings")) {
    for (auto const& [gameId, gameValue] : *gameSettingsOpt) {
      GameSettings.emplace(gameId, DeserializeGameConfig(gameId, gameValue));
    }
  }
  if (auto advancedSettingsOpt =
          toml::find<std::optional<decltype(AdvancedSettings)>>(
              tomlConfig, "AdvancedSettings")) {
    AdvancedSettings = std::move(*advancedSettingsOpt);
  }

  if (ActiveGame.empty())
    ActiveGame = toml::find_or(tomlConfig, "ActiveGame", "");

  for (auto& [gameProfile, gameDef] : Profile::GameDefinitions) {
    if (gameDef.Hidden) continue;
    auto [itr, inserted] =
        GameSettings.try_emplace(gameProfile, UserConfig::GameConfig{});
    if (inserted) {
      itr->second.Extra = MakeDefaultExtra(gameProfile);
    }
  }
};

GameConfig& ActiveGameSettings() { return GameSettings[GetActiveGame()]; }

std::string const& GetActiveGame() {
  if (!ActiveGameOverride.empty()) return ActiveGameOverride;
  return ActiveGame;
}

void SetActiveGame(std::string activeGame) {
  ActiveGame = std::move(activeGame);
}

std::optional<std::string_view> GetPatchProfile() {
  if (!PatchProfileOverride.empty()) {
    return PatchProfileOverride;
  }

  auto const& gameSettings = ActiveGameSettings();
  if (gameSettings.UsePatch) {
    return gameSettings.PatchProfile;
  }

  return std::nullopt;
}

void WriteUserConfig() {
  // Reparse to preserve comments and formatting
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  auto userConfigFile = openTomlFile(CF::READ | CF::WRITE);
  std::string fileContents(userConfigFile->Meta.Size, '\0');
  userConfigFile->Read(fileContents.data(), userConfigFile->Meta.Size);

  auto parseResult =
      toml::try_parse_str<toml::ordered_type_config>(fileContents);
  if (parseResult.is_err()) {
    ImpLog(LogLevel::Fatal, LogChannel::Config,
           "Error parsing user config: \n");
    for (const auto& err : parseResult.as_err()) {
      ImpLog(LogLevel::Fatal, LogChannel::Config, "\t{}\n",
             toml::format_error(err));
    }
    throw std::runtime_error("Failed to parse user configuration file.");
  }

  auto&& tomlConfig = parseResult.as_ok();
  tomlConfig["ActiveGame"] = ActiveGame;
  toml::value gameSettingsTable = toml::table{};
  for (auto const& [gameId, cfg] : GameSettings) {
    gameSettingsTable[gameId] = SerializeGameConfig(gameId, cfg);
  }
  tomlConfig["GameSettings"] = gameSettingsTable;
  tomlConfig["CommonSettings"] = CommonSettings;
  tomlConfig["AdvancedSettings"] = AdvancedSettings;

  std::string tomlContents = toml::format(tomlConfig);
  userConfigFile->Seek(0, RW_SEEK_SET);
  userConfigFile->Write(tomlContents.data(), tomlContents.size());
}

}  // namespace Impacto::UserConfig
