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

template <>
struct from<Impacto::LogChannel> {
  static Impacto::LogChannel from_toml(const toml::value& v) {
    auto enumStr = toml::get<std::string_view>(v);

    std::optional<Impacto::LogChannel> channelOpt =
        Impacto::StringToChannel(enumStr);

    if (!channelOpt) {
      std::string errStr = fmt::format(
          "Expected one of {}, All!\n", enumStr,
          fmt::join(magic_enum::enum_names<Impacto::LogChannel>(), ", "));
      auto err = toml::make_error_info(errStr, v, "but got invalid value");
      throw toml::type_error(toml::format_error("", err), v.location());
    }

    return *channelOpt;
  }
};

template <>
struct into<Impacto::LogChannel> {
  template <typename TC>
  static toml::basic_value<TC> into_toml(const Impacto::LogChannel& v) {
    using value_type = toml::basic_value<TC>;
    using string_type = typename value_type::string_type;

    return value_type(string_type{Impacto::ChannelToString(v)});
  }
};

}  // namespace toml

// clang-format off
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
  Impacto::UserConfig::GameConfig,
  ResolutionWidth,
  ResolutionHeight,
  PatchProfile,
  UsePatch,
  Display
)

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
  Impacto::UserConfig::Config, 
  ResolutionWidth, 
  ResolutionHeight,
  LogFile,
  LogLvl,
  LogChannels,
  LoggingToConsole,
  LoggingToFile
)

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
  Impacto::UserConfig::AdvancedConfig,
  ActiveRenderer,
  VideoPlayer,
  ActiveAudioBackend,
  SubtitleAssBackend,
  SubtitleTextBackend,
  SubtitleBmpBackend
)

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
  Impacto::UserConfig::CHLCCEnhancements,
  DelusionMousePatch
)
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
  Impacto::UserConfig::CCLCCEnhancements,
  DelusionMousePatch
)
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
  Impacto::UserConfig::EnhancementsConfig,
  SubtitleConfig,
  CloseBacklogWhenReachedEnd,
  CHLCC,
  CCLCC
)

// clang-format on

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
  ImpLog(LogLevel::Debug, LogChannel::Config, "Opening config file \"{}\"\n",
         UserConfigPath);

  userConfigFile.reset(static_cast<Io::PhysicalFileStream*>(stream));
  return userConfigFile;
}

void Configure() {
  constexpr auto fileName = "userconfig.toml";
  std::string fileContents;
  {
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
    fileContents.resize(userConfigFile->Meta.Size, '\0');
    userConfigFile->Read(fileContents.data(), userConfigFile->Meta.Size);
  }

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
  ImpLog(LogLevel::Info, LogChannel::Config, "Loading user config\n");

  if (auto commonSettingsOpt =
          toml::find<std::optional<decltype(CommonSettings)>>(
              tomlConfig, "CommonSettings")) {
    CommonSettings = std::move(*commonSettingsOpt);
    LogInitFile();
  }
  if (auto gameSettingsOpt = toml::find<std::optional<decltype(GameSettings)>>(
          tomlConfig, "GameSettings")) {
    GameSettings = std::move(*gameSettingsOpt);
  }

  if (auto advancedSettingsOpt =
          toml::find<std::optional<decltype(AdvancedSettings)>>(
              tomlConfig, "AdvancedSettings")) {
    AdvancedSettings = std::move(*advancedSettingsOpt);
  }

  if (auto enhancementsSettingsOpt =
          toml::find<std::optional<decltype(EnhancementsSettings)>>(
              tomlConfig, "EnhancementsSettings")) {
    EnhancementsSettings = std::move(*enhancementsSettingsOpt);
  }

  for (auto& [gameProfile, gameDef] : Profile::GameDefinitions) {
    if (gameDef.Hidden) continue;
    GameSettings.try_emplace(gameProfile, UserConfig::GameConfig{});
  }
  ImpLog(LogLevel::Info, LogChannel::Config, "Loaded user config\n");
};

void RestoreSettingsDefaults() {
  // Enhancements is excluded since that's in a separate tab
  CommonSettings = decltype(CommonSettings){};
  AdvancedSettings = decltype(AdvancedSettings){};
  GameSettings = decltype(GameSettings){};

  for (auto& [gameProfile, gameDef] : Profile::GameDefinitions) {
    if (gameDef.Hidden) continue;
    GameSettings.try_emplace(gameProfile, UserConfig::GameConfig{});
  }

  LogInitFile();
}

GameConfig& ActiveGameSettings() { return GameSettings[GetActiveGame()]; }

std::string const& GetActiveGame() { return ActiveGame; }

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
  ImpLog(LogLevel::Info, LogChannel::Config, "Saving user config\n");

  // Reparse to preserve comments and formatting
  using CF = Io::PhysicalFileStream::CreateFlagsMode;

  ImpLog(LogLevel::Debug, LogChannel::Config, "Reparsing config file\n");
  auto userConfigFile = openTomlFile(CF::READ | CF::WRITE);
  std::string fileContents(userConfigFile->Meta.Size, '\0');
  userConfigFile->Read(fileContents.data(), userConfigFile->Meta.Size);

  auto parseResult = toml::try_parse_str<toml::ordered_type_config>(
      fileContents, toml::spec::v(1, 1, 0));
  if (parseResult.is_err()) {
    ImpLog(LogLevel::Fatal, LogChannel::Config,
           "Error parsing user config: \n");
    for (const auto& err : parseResult.as_err()) {
      ImpLog(LogLevel::Fatal, LogChannel::Config, "\t{}\n",
             toml::format_error(err));
    }
    throw std::runtime_error("Failed to parse user configuration file.");
  }

  ImpLog(LogLevel::Debug, LogChannel::Config, "Parsed user config from file\n");
  auto&& tomlConfig = parseResult.as_ok();

  ImpLog(LogLevel::Debug, LogChannel::Config,
         "Serializing current configuration\n");
  tomlConfig["GameSettings"] = GameSettings;
  tomlConfig["CommonSettings"] = CommonSettings;
  tomlConfig["AdvancedSettings"] = AdvancedSettings;
  tomlConfig["EnhancementsSettings"] = EnhancementsSettings;

  ImpLog(LogLevel::Debug, LogChannel::Config, "Formatting TOML String\n");
  std::string tomlContents = toml::format(tomlConfig, toml::spec::v(1, 1, 0));

  ImpLog(LogLevel::Debug, LogChannel::Config, "Writing user config to file\n");

  userConfigFile->Seek(0, RW_SEEK_SET);
  userConfigFile->Write(tomlContents.data(), tomlContents.size());
  ImpLog(LogLevel::Info, LogChannel::Config, "Saved user config\n");
}

}  // namespace Impacto::UserConfig
