#include "userconfig.h"

#include <optional>

#include <toml.hpp>
#include "io/physicalfilestream.h"
#include "profile/gamedefinitions.h"
#include "log.h"

namespace toml {
using Impacto::UserConfig::Config;
using Impacto::UserConfig::GameConfig;

static bool AssignIfExists(toml::value const& v, std::string const& configKey,
                           auto& member) {
  using MemberType = std::decay_t<decltype(member)>;

  using namespace Impacto;

  bool exists = false;
  try {
    if constexpr (std::is_enum_v<MemberType>) {
      using MagicEnumRange = magic_enum::customize::enum_range<MemberType>;

      std::optional<MemberType> enumOpt;
      auto enumStr = toml::find<std::optional<std::string>>(v, configKey);
      exists = enumStr.has_value();
      if (enumStr) {
        if constexpr (MagicEnumRange::is_flags)
          enumOpt = magic_enum::enum_flags_cast<MemberType>(*enumStr);
        else
          enumOpt = magic_enum::enum_cast<MemberType>(*enumStr);

        if (!enumOpt) {
          ImpLog(LogLevel::Fatal, LogChannel::Config,
                 "Member {} in config has unexpected value: {}\n", configKey,
                 *enumStr);
          throw std::runtime_error("Invalid value in config");
        }

        member = *enumOpt;
      }
    } else {
      auto tomlMember = toml::find<std::optional<MemberType>>(v, configKey);
      exists = tomlMember.has_value();
      if (tomlMember) {
        member = *tomlMember;
      }
    }
  } catch (toml::type_error const& e) {
    ImpLog(LogLevel::Error, LogChannel::Config,
           "Member {} in config has unexpected type: {}\n", configKey,
           e.what());
    throw(e);
  }
  return exists;
};

template <>
struct from<GameConfig> {
  static GameConfig from_toml(const toml::value& v) {
    GameConfig gameConfig;
    AssignIfExists(v, "UsePatch", gameConfig.UsePatch);
    AssignIfExists(v, "PatchProfile", gameConfig.PatchProfile);

    return gameConfig;
  }
};
template <>
struct from<Config> {
  static Config from_toml(const toml::value& v) {
    Config config;
    AssignIfExists(v, "ResolutionWidth", config.ResolutionWidth);
    AssignIfExists(v, "ResolutionHeight", config.ResolutionHeight);
    AssignIfExists(v, "UseNativeGameResolution",
                   config.UseNativeGameResolution);
    AssignIfExists(v, "FullScreen", config.Fullscreen);
    AssignIfExists(v, "SubtitleConfig", config.SubtitleConfig);
    AssignIfExists(v, "CloseBacklogWhenReachedEnd",
                   config.CloseBacklogWhenReachedEnd);

    return config;
  }
};

template <>
struct into<GameConfig> {
  template <typename TC>
  static toml::basic_value<TC> into_toml(const GameConfig& f) {
    using value_type = toml::basic_value<TC>;
    using table_type = typename value_type::table_type;

    return value_type(table_type{
        {"UsePatch", f.UsePatch},
        {"PatchProfile", f.PatchProfile},
    });
  }
};
template <>
struct into<Config> {
  template <typename TC>
  static toml::basic_value<TC> into_toml(const Config& config) {
    using value_type = toml::basic_value<TC>;
    using table_type = typename value_type::table_type;

    return value_type(table_type{
        {"ResolutionWidth", config.ResolutionWidth},
        {"ResolutionHeight", config.ResolutionWidth},
        {"UseNativeGameResolution", config.UseNativeGameResolution},
        {"FullScreen", config.Fullscreen},
        {"SubtitleConfig", magic_enum::enum_flags_name(config.SubtitleConfig)},
        {"CloseBacklogWhenReachedEnd", config.CloseBacklogWhenReachedEnd},
    });
  }
};

}  // namespace toml

namespace Impacto::UserConfig {
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

  auto parseResult = toml::try_parse_str(fileContents);
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

  if (auto commonSettingsOpt =
          toml::find<std::optional<decltype(CommonSettings)>>(
              tomlConfig, "CommonSettings")) {
    CommonSettings = std::move(*commonSettingsOpt);
  }
  if (auto gameSettingsOpt = toml::find<std::optional<decltype(GameSettings)>>(
          tomlConfig, "GameSettings")) {
    GameSettings = std::move(*gameSettingsOpt);
  }

  if (ActiveGame.empty())
    ActiveGame = toml::find_or(tomlConfig, "ActiveGame", "");

  for (auto& [gameProfile, gameDef] : Profile::GameDefinitions) {
    GameSettings.try_emplace(gameProfile, UserConfig::GameConfig{});
  }

  WriteUserConfig();
};

GameConfig& ActiveGameSettings() { return GameSettings.at(ActiveGame); }

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

  auto parseResult = toml::try_parse_str(fileContents);
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
  tomlConfig["GameSettings"] = GameSettings;
  tomlConfig["CommonSettings"] = CommonSettings;

  std::string tomlContents = toml::format(tomlConfig);
  userConfigFile->Seek(0, RW_SEEK_SET);
  userConfigFile->Write(tomlContents.data(), tomlContents.size());
}

}  // namespace Impacto::UserConfig
