#include "profile.h"

#include <algorithm>
#include <ranges>
#include <ankerl/unordered_dense.h>
#include "profile_internal.h"
#include "../io/physicalfilestream.h"
#include "../log.h"

#include "basepaths.h"
#include "gamedefinitions.h"
#include "userconfig.h"
#include "ui/backlogmenu.h"
#include "dialogue.h"
#include "configsystem.h"
#include "subtitle.h"
#include "game.h"
#include "patch.h"
#include "../text/textpage.h"
#include "../text/text.h"
#include "../game.h"
#include "../ui/ui.h"
#include "../data/savesystem.h"
#include "../data/achievementsystem.h"
#include "../data/tipssystem.h"
#include "../hud/datedisplay.h"
#include "../hud/waiticondisplay.h"
#include "../hud/autoicondisplay.h"
#include "../hud/skipicondisplay.h"
#include "../hud/tipsnotification.h"
#include "../profile/hud/saveicon.h"
#include "../inputsystem.h"

namespace Impacto {
namespace Profile {

static ankerl::unordered_dense::set<std::string, string_hash, std::equal_to<>>
    IncludedFiles;

static void RunLuaScriptBuffer(std::span<char const> buffer) {
  if (luaL_loadbuffer(LuaState, buffer.data(), buffer.size(), buffer.data())) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Lua profile compile error: {:s}\n", lua_tostring(LuaState, -1));
    lua_close(LuaState);
    exit(1);
  }
  if (lua_pcall(LuaState, 0, 0, 0)) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Lua profile execute error: {:s}\n", lua_tostring(LuaState, -1));
    lua_close(LuaState);
    exit(1);
  }
}

static void RunLuaScript(const char* path) {
  ImpLog(LogLevel::Info, LogChannel::Profile, "Executing lua script {:s}\n",
         path);
  if (luaL_dofile(LuaState, path)) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Lua profile compile error: {:s}\n", lua_tostring(LuaState, -1));
    exit(1);
  }
  ImpLog(LogLevel::Info, LogChannel::Profile, "Lua profile execute success\n");
}

static int LuaPrint(lua_State* ctx) {
  ImpLog(LogLevel::Info, LogChannel::Profile, "Lua: {:s}\n",
         lua_tostring(ctx, 1));
  return 0;
}

static int LuaInclude(lua_State* ctx) {
  auto file = lua_tostring(ctx, 1);

  if (IncludedFiles.find(file) != IncludedFiles.end()) {
    ImpLog(LogLevel::Debug, LogChannel::Profile,
           "File {:s} already included, skipping...\n", file);
    return 0;
  }

  IncludedFiles.insert(file);
  ImpLog(LogLevel::Debug, LogChannel::Profile, "Including {:s}\n", file);

  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(file, &stream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not open {:s}\n", file);
    return 0;
  }

  char const prefix[] = "do\n";
  char const suffix[] = "\nend";

  size_t script_len = strlen(prefix) + stream->Meta.Size + strlen(suffix) + 1;
  auto script = std::make_unique<char[]>(script_len);

  script[script_len - 1] = '\0';
  memcpy(script.get(), prefix, strlen(prefix));

  int64_t len = stream->Read(script.get() + strlen(prefix), stream->Meta.Size);

  if (len < 0) {
    delete stream;

    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not open {:s}\n", file);
    return 0;
  }

  len += strlen(prefix);
  memcpy(script.get() + len, suffix, strlen(suffix));
  len += strlen(suffix);

  RunLuaScriptBuffer({script.get(), static_cast<size_t>(len)});

  delete stream;

  lua_pop(ctx, -1);

  return 0;
}

static int LuaGetLanguage(lua_State* ctx) {
  if (UserConfig::GameSettings.empty()) {
    ImpLog(
        LogLevel::Error, LogChannel::Profile,
        "getLanguage called in profile before ready, returning empty string");
    lua_pushstring(ctx, "");
    return 1;
  }
  auto const& lang = UserConfig::ActiveGameSettings().Language;
  lua_pushstring(ctx, lang.c_str());
  return 1;
}

template <typename Enum>
static void DefineEnum(lua_State* ctx) {
  lua_createtable(ctx, 0, 0);
  const auto enumTypeName = std::string(magic_enum::enum_type_name<Enum>());
  for (auto&& [value, name] : magic_enum::enum_entries<Enum>()) {
    const auto fieldName = std::string(name);
    if constexpr (std::is_floating_point_v<std::underlying_type_t<Enum>>) {
      lua_pushnumber(ctx, to_underlying(value));
    } else {
      lua_pushinteger(ctx, to_underlying(value));
    }
    lua_setfield(ctx, -2, fieldName.c_str());
  }
  lua_setglobal(ctx, enumTypeName.c_str());
}

static void DefineEnums() {
  DefineEnum<RendererType>(LuaState);
  DefineEnum<VideoPlayerType>(LuaState);
  DefineEnum<AudioBackendType>(LuaState);
  DefineEnum<TextAlignment>(LuaState);
  DefineEnum<GameFeature>(LuaState);
  DefineEnum<CharacterTypeFlags>(LuaState);
  DefineEnum<Vm::InstructionSet>(LuaState);
  DefineEnum<Impacto::Game::DrawComponentType>(LuaState);
  DefineEnum<SaveSystem::SaveDataType>(LuaState);
  DefineEnum<AchievementSystem::AchievementDataType>(LuaState);
  DefineEnum<TipsSystem::TipsSystemType>(LuaState);
  DefineEnum<UI::CommonMenuType>(LuaState);
  DefineEnum<UI::SystemMenuType>(LuaState);
  DefineEnum<UI::TitleMenuType>(LuaState);
  DefineEnum<UI::SaveMenuType>(LuaState);
  DefineEnum<UI::OptionsMenuType>(LuaState);
  DefineEnum<UI::TrophyMenuType>(LuaState);
  DefineEnum<UI::HelpMenuType>(LuaState);
  DefineEnum<BacklogMenu::BacklogMenuType>(LuaState);
  DefineEnum<BacklogMenu::EntryHighlightLocationType>(LuaState);
  DefineEnum<UI::TipsMenuType>(LuaState);
  DefineEnum<UI::LibraryMenuType>(LuaState);
  DefineEnum<UI::ClearListMenuType>(LuaState);
  DefineEnum<UI::AlbumMenuType>(LuaState);
  DefineEnum<UI::MusicMenuType>(LuaState);
  DefineEnum<UI::MovieMenuType>(LuaState);
  DefineEnum<UI::ActorsVoiceMenuType>(LuaState);
  DefineEnum<DateDisplay::DateDisplayType>(LuaState);
  DefineEnum<WaitIconDisplay::WaitIconType>(LuaState);
  DefineEnum<AutoIconDisplay::AutoIconType>(LuaState);
  DefineEnum<SkipIconDisplay::SkipIconType>(LuaState);
  DefineEnum<SaveIcon::SaveIconType>(LuaState);
  DefineEnum<Dialogue::NametagType>(LuaState);
  DefineEnum<TipsNotification::TipsNotificationType>(LuaState);
  DefineEnum<DialogueBoxType>(LuaState);
  DefineEnum<UI::SysMesBoxType>(LuaState);
  DefineEnum<FontType>(LuaState);
  DefineEnum<LKMVersion>(LuaState);
  DefineEnum<ShaderProgramType>(LuaState);
  DefineEnum<ConfigSystem::AutoQuickSaveType>(LuaState);
  DefineEnum<UI::GameSpecificType>(LuaState);
  DefineEnum<Patch::DateFormatType>(LuaState);
  DefineEnum<SubtitleAssBackendType>(LuaState);
  DefineEnum<SubtitleBmpBackendType>(LuaState);
  DefineEnum<SubtitleTextBackendType>(LuaState);
  DefineEnum<Subtitle::SubtitleType>(LuaState);
  DefineEnum<Subtitle::SubtitleConfigType>(LuaState);
  DefineEnum<Input::KeyboardScanCode>(LuaState);
  DefineEnum<Input::ControllerButton>(LuaState);
  DefineEnum<Input::ControllerAxis>(LuaState);
  DefineEnum<TextModeInfo::NameDispModeType>(LuaState);
  DefineEnum<TextModeInfo::NameAlignmentType>(LuaState);
}

void Init() {
  LuaState = luaL_newstate();
  // Set up API
  luaL_openlibs(LuaState);
  lua_pushcfunction(LuaState, LuaPrint);
  lua_setglobal(LuaState, "print");
  lua_pushcfunction(LuaState, LuaInclude);
  lua_setglobal(LuaState, "include");
  lua_pushcfunction(LuaState, LuaGetLanguage);
  lua_setglobal(LuaState, "getLanguage");

  // Root profile object
  lua_createtable(LuaState, 0, 0);
  lua_setglobal(LuaState, "root");

  // Enums /sigh
  DefineEnums();
}

void Configure() {
  const auto setupScriptPath = [](std::string const& fileName,
                                  std::string& path) {
    std::filesystem::path configDir = Io::GetPlatformConfigDir();
    if (path.empty()) {
      path = (configDir / fileName).string();
    }
  };

  setupScriptPath("baseconfig.lua", BasePathsPath);
  setupScriptPath("gamedefinitions.lua", GameDefinitionsPath);
  setupScriptPath("userconfig.lua", UserConfigPath);

  // Order dependent!
  RunLuaScript(BasePathsPath.c_str());
  RunLuaScript(GameDefinitionsPath.c_str());
  RunLuaScript(UserConfigPath.c_str());

  // Push the global onto the stack to load all the values later
  lua_getglobal(LuaState, "root");

  BasePaths::Configure();
  GameDefinition::Configure();
  UserConfig::Configure();

  if (UserConfig::UsePatchOverride) {
    UserConfig::GameSettings.at(UserConfig::ActiveGame).UsePatch = true;
  }

  if (!UserConfig::LanguageOverride.empty()) {
    UserConfig::GameSettings.at(UserConfig::ActiveGame).Language =
        UserConfig::LanguageOverride;
  }
  auto const& activeGameSettings = UserConfig::ActiveGameSettings();
  if (BasePaths::RootPatchesDir.empty() && activeGameSettings.UsePatch) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Patch is enabled but no patch directory is specified\n");
    exit(1);
  }
  auto const& activeGameDef = GameDefinitions.at(UserConfig::ActiveGame);

  std::string const& gameProfilePath = activeGameDef.GameProfile;
  RunLuaScript(gameProfilePath.c_str());
  if (activeGameSettings.UsePatch) {
    auto patchProfilePathItr =
        activeGameDef.Patch.find(activeGameSettings.Language);
    if (patchProfilePathItr == activeGameDef.Patch.end()) {
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Patch is enabled but patch.lua path is missing for language {} "
             "in game {} definition\n",
             activeGameSettings.Language, UserConfig::ActiveGame);
      exit(1);
    }
    RunLuaScript(patchProfilePathItr->second.c_str());
  }
  ImpLog(LogLevel::Info, LogChannel::Profile,
         "Profile for {:s} loaded successfully.\n", UserConfig::ActiveGame);

  // Push the global onto the stack to load all the values later
  lua_getglobal(LuaState, "root");
}

void ClearProfile() { ClearProfileInternal(); }

}  // namespace Profile
}  // namespace Impacto
