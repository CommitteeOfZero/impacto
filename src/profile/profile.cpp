#include "profile.h"
#include "profile_internal.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../renderer/renderer.h"
#include <ankerl/unordered_dense.h>

#include "ui/backlogmenu.h"
#include "dialogue.h"
#include "../font.h"
#include "../text.h"
#include "../game.h"
#include "../ui/ui.h"
#include "../vm/vm.h"
#include "../data/savesystem.h"
#include "../data/achievementsystem.h"
#include "../data/tipssystem.h"
#include "../hud/datedisplay.h"
#include "../hud/waiticondisplay.h"
#include "../hud/autoicondisplay.h"
#include "../hud/skipicondisplay.h"
#include "../hud/dialoguebox.h"
#include "../hud/tipsnotification.h"
#include "../hud/delusiontrigger.h"
#include "../renderer/3d/model.h"

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
static void DefineEnumDouble(lua_State* ctx) {
  lua_createtable(ctx, 0, 0);
  for (Enum value : Enum::_values()) {
    lua_pushnumber(ctx, value._value);
    lua_setfield(ctx, -2, value._to_string());
  }
  lua_setglobal(ctx, Enum::_name());
}

template <typename Enum>
static void DefineEnumInt(lua_State* ctx) {
  lua_createtable(ctx, 0, 0);
  for (Enum value : Enum::_values()) {
    lua_pushinteger(ctx, value._value);
    lua_setfield(ctx, -2, value._to_string());
  }
  lua_setglobal(ctx, Enum::_name());
}

template <typename Enum>
static void DefineEnumUint(lua_State* ctx) {
  lua_createtable(ctx, 0, 0);
  for (Enum value : Enum::_values()) {
    lua_pushinteger(ctx, value._value);
    lua_setfield(ctx, -2, value._to_string());
  }
  lua_setglobal(ctx, Enum::_name());
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
  DefineEnumInt<RendererType>(LuaState);
  DefineEnumInt<VideoPlayerType>(LuaState);
  DefineEnumInt<AudioBackendType>(LuaState);
  DefineEnumInt<TextAlignment>(LuaState);
  DefineEnumInt<GameFeature>(LuaState);
  DefineEnumInt<CharacterTypeFlags>(LuaState);
  DefineEnumInt<Vm::InstructionSet>(LuaState);
  DefineEnumUint<Game::DrawComponentType>(LuaState);
  DefineEnumInt<SaveSystem::SaveDataType>(LuaState);
  DefineEnumInt<AchievementSystem::AchievementDataType>(LuaState);
  DefineEnumInt<TipsSystem::TipsSystemType>(LuaState);
  DefineEnumInt<UI::SystemMenuType>(LuaState);
  DefineEnumInt<UI::TitleMenuType>(LuaState);
  DefineEnumInt<UI::SaveMenuType>(LuaState);
  DefineEnumInt<UI::OptionsMenuType>(LuaState);
  DefineEnumInt<UI::TrophyMenuType>(LuaState);
  DefineEnumInt<UI::HelpMenuType>(LuaState);
  DefineEnumInt<BacklogMenu::BacklogMenuType>(LuaState);
  DefineEnumInt<BacklogMenu::EntryHighlightLocationType>(LuaState);
  DefineEnumInt<UI::TipsMenuType>(LuaState);
  DefineEnumInt<UI::LibraryMenuType>(LuaState);
  DefineEnumInt<UI::ClearListMenuType>(LuaState);
  DefineEnumInt<UI::AlbumMenuType>(LuaState);
  DefineEnumInt<UI::MusicMenuType>(LuaState);
  DefineEnumInt<UI::MovieMenuType>(LuaState);
  DefineEnumInt<UI::ActorsVoiceMenuType>(LuaState);
  DefineEnumInt<DateDisplay::DateDisplayType>(LuaState);
  DefineEnumInt<WaitIconDisplay::WaitIconType>(LuaState);
  DefineEnumInt<AutoIconDisplay::AutoIconType>(LuaState);
  DefineEnumInt<SkipIconDisplay::SkipIconType>(LuaState);
  DefineEnumInt<TipsNotification::TipsNotificationType>(LuaState);
  DefineEnumInt<DelusionTrigger::DelusionTriggerType>(LuaState);
  DefineEnumInt<DialogueBoxType>(LuaState);
  DefineEnumInt<UI::SysMesBoxType>(LuaState);
  DefineEnumInt<FontType>(LuaState);
  DefineEnumInt<LKMVersion>(LuaState);
  DefineEnumInt<Dialogue::REVNameLocationType>(LuaState);
  DefineEnumInt<ShaderProgramType>(LuaState);

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
