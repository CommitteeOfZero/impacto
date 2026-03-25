#include "profile.h"
#include "profile_internal.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include <ankerl/unordered_dense.h>

#include "ui/backlogmenu.h"
#include "dialogue.h"
#include "configsystem.h"
#include "subtitle.h"
#include "game.h"
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

static ankerl::unordered_dense::set<std::string> IncludedFiles;

static int LuaPrint(lua_State* ctx) {
  ImpLog(LogLevel::Info, LogChannel::Profile, "Lua: {:s}\n",
         lua_tostring(ctx, 1));
  return 0;
}

static int LuaInclude(lua_State* ctx) {
  auto fileName = lua_tostring(ctx, 1);
  std::string file = "profiles/" + std::string(fileName);
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
  char* script = (char*)malloc(script_len);

  if (script == NULL) {
    delete stream;

    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not allocate memory for script: {:s}", file);
    return 0;
  }

  script[script_len - 1] = '\0';
  memcpy(script, prefix, strlen(prefix));

  int64_t len = stream->Read(script + strlen(prefix), stream->Meta.Size);

  if (len < 0) {
    free(script);
    delete stream;

    ImpLog(LogLevel::Error, LogChannel::Profile, "Could not open {:s}\n", file);
    return 0;
  }

  len += strlen(prefix);
  memcpy(script + len, suffix, strlen(suffix));
  len += strlen(suffix);

  if (luaL_loadbuffer(LuaState, script, len, script)) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Lua profile compile error: {:s}\n", lua_tostring(ctx, -1));
    lua_close(LuaState);
    exit(1);
  }
  if (lua_pcall(ctx, 0, 0, 0)) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Lua profile execute error: {:s}\n", lua_tostring(ctx, -1));
    lua_close(LuaState);
    exit(1);
  }

  free(script);
  delete stream;

  lua_pop(ctx, -1);

  return 0;
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

void MakeLuaProfile(std::string const& name) {
  Io::Stream* stream;
  IoError err =
      Io::PhysicalFileStream::Create("profiles/" + name + "/game.lua", &stream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Could not open profiles/{:s}/game.lua\n", name);
    exit(1);
  }

  char* script = (char*)malloc(stream->Meta.Size + 1);
  if (script == NULL) {
    delete stream;

    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not allocate memory for script: profiles/{:s}/game.lua",
           name);
    exit(1);
  }

  int64_t len = stream->Read(script, stream->Meta.Size);
  if (len < 0) {
    delete stream;
    free(script);

    ImpLog(LogLevel::Fatal, LogChannel::Profile,
           "Could not open profiles/{:s}/game.lua\n", name);
    exit(1);
  }
  script[len] = '\0';

  LuaState = luaL_newstate();
  // Set up API
  luaL_openlibs(LuaState);
  lua_pushcfunction(LuaState, LuaPrint);
  lua_setglobal(LuaState, "print");
  lua_pushcfunction(LuaState, LuaInclude);
  lua_setglobal(LuaState, "include");

  // Root profile object
  lua_createtable(LuaState, 0, 0);
  lua_setglobal(LuaState, "root");

  // Enums /sigh
  DefineEnum<RendererType>(LuaState);
  DefineEnum<VideoPlayerType>(LuaState);
  DefineEnum<AudioBackendType>(LuaState);
  DefineEnum<TextAlignment>(LuaState);
  DefineEnum<GameFeature>(LuaState);
  DefineEnum<CharacterTypeFlags>(LuaState);
  DefineEnum<Vm::InstructionSet>(LuaState);
  DefineEnum<Game::DrawComponentType>(LuaState);
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
  DefineEnum<Dialogue::REVNameLocationType>(LuaState);
  DefineEnum<ShaderProgramType>(LuaState);
  DefineEnum<ConfigSystem::AutoQuickSaveType>(LuaState);
  DefineEnum<UI::GameSpecificType>(LuaState);
  DefineEnum<DateFormatType>(LuaState);
  DefineEnum<SubtitleAssBackendType>(LuaState);
  DefineEnum<SubtitleBmpBackendType>(LuaState);
  DefineEnum<SubtitleTextBackendType>(LuaState);
  DefineEnum<Subtitle::SubtitleType>(LuaState);
  DefineEnum<SubtitleConfigType>(LuaState);
  DefineEnum<Input::KeyboardScanCode>(LuaState);
  DefineEnum<Input::ControllerButton>(LuaState);
  DefineEnum<Input::ControllerAxis>(LuaState);

  ImpLog(LogLevel::Info, LogChannel::Profile, "Starting profile {:s}\n", name);

  if (luaL_loadbuffer(LuaState, script, len, name.c_str())) {
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

  delete stream;
  free(script);

  // Push the global onto the stack to load all the values later
  lua_getglobal(LuaState, "root");

  ImpLog(LogLevel::Info, LogChannel::Profile, "Lua profile execute success\n");
}

void ClearProfile() { ClearProfileInternal(); }

}  // namespace Profile
}  // namespace Impacto
