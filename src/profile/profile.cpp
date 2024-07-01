#include "profile.h"
#include "profile_internal.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../renderer/renderer.h"
#include <flat_hash_map.hpp>

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
#include "../hud/dialoguebox.h"
#include "../hud/tipsnotification.h"
#include "../hud/delusiontrigger.h"
#include "../renderer/3d/model.h"

namespace Impacto {
namespace Profile {

static ska::flat_hash_set<std::string> IncludedFiles;

static int LuaPrint(lua_State* ctx) {
  ImpLog(LL_Info, LC_Profile, "Lua: %s\n", lua_tostring(ctx, 1));
  return 0;
}

static int LuaInclude(lua_State* ctx) {
  auto fileName = lua_tostring(ctx, 1);
  std::string file = "profiles/" + std::string(fileName);
  if (IncludedFiles.find(file) != IncludedFiles.end()) {
    ImpLog(LL_Debug, LC_Profile, "File %s already included, skipping...\n",
           file.c_str());
    return 0;
  }

  IncludedFiles.insert(file);
  ImpLog(LL_Debug, LC_Profile, "Including %s\n", file.c_str());

  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(file, &stream);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_Profile, "Could not open %s\n", file.c_str());
    return 0;
  }

  char const prefix[] = "do\n";
  char const suffix[] = "\nend";

  size_t script_len = strlen(prefix) + stream->Meta.Size + strlen(suffix) + 1;
  char* script = (char*)malloc(script_len);

  if (script == NULL) {
    delete stream;

    ImpLog(LL_Error, LC_Profile, "Could not allocate memory for script: %s",
           file.c_str());
    return 0;
  }

  script[script_len - 1] = '\0';
  memcpy(script, prefix, strlen(prefix));

  int64_t len = stream->Read(script + strlen(prefix), stream->Meta.Size);

  if (len < 0) {
    free(script);
    delete stream;

    ImpLog(LL_Error, LC_Profile, "Could not open %s\n", file.c_str());
    return 0;
  }

  len += strlen(prefix);
  memcpy(script + len, suffix, strlen(suffix));
  len += strlen(suffix);

  if (luaL_loadbuffer(LuaState, script, len, script)) {
    ImpLog(LL_Fatal, LC_Profile, "Lua profile compile error: %s\n",
           lua_tostring(ctx, -1));
    lua_close(LuaState);
    exit(0);
  }
  if (lua_pcall(ctx, 0, 0, 0)) {
    ImpLog(LL_Fatal, LC_Profile, "Lua profile execute error: %s\n",
           lua_tostring(ctx, -1));
    lua_close(LuaState);
    exit(0);
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
    ImpLog(LL_Fatal, LC_Profile, "Could not open profiles/%s/game.lua\n",
           name.c_str());
    exit(0);
  }

  char* script = (char*)malloc(stream->Meta.Size + 1);
  if (script == NULL) {
    delete stream;

    ImpLog(LL_Error, LC_Profile,
           "Could not allocate memory for script: profiles/%s/game.lua",
           name.c_str());
    exit(0);
  }

  int64_t len = stream->Read(script, stream->Meta.Size);
  if (len < 0) {
    delete stream;
    free(script);

    ImpLog(LL_Fatal, LC_Profile, "Could not open profiles/%s/game.lua\n",
           name.c_str());
    exit(0);
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
  DefineEnumInt<UI::TipsMenuType>(LuaState);
  DefineEnumInt<UI::ClearListMenuType>(LuaState);
  DefineEnumInt<UI::AlbumMenuType>(LuaState);
  DefineEnumInt<UI::MusicMenuType>(LuaState);
  DefineEnumInt<UI::MovieMenuType>(LuaState);
  DefineEnumInt<UI::ActorsVoiceMenuType>(LuaState);
  DefineEnumInt<DateDisplay::DateDisplayType>(LuaState);
  DefineEnumInt<WaitIconDisplay::WaitIconType>(LuaState);
  DefineEnumInt<TipsNotification::TipsNotificationType>(LuaState);
  DefineEnumInt<DelusionTrigger::DelusionTriggerType>(LuaState);
  DefineEnumInt<DialogueBoxType>(LuaState);
  DefineEnumInt<UI::SysMesBoxType>(LuaState);
  DefineEnumInt<FontType>(LuaState);
  DefineEnumInt<LKMVersion>(LuaState);

  ImpLog(LL_Info, LC_Profile, "Starting profile %s\n", name.c_str());

  if (luaL_loadbuffer(LuaState, script, len, name.c_str())) {
    ImpLog(LL_Fatal, LC_Profile, "Lua profile compile error: %s\n",
           lua_tostring(LuaState, -1));
    lua_close(LuaState);
    exit(0);
  }
  if (lua_pcall(LuaState, 0, 0, 0)) {
    ImpLog(LL_Fatal, LC_Profile, "Lua profile execute error: %s\n",
           lua_tostring(LuaState, -1));
    lua_close(LuaState);
    exit(0);
  }

  delete stream;
  free(script);

  // Push the global onto the stack to load all the values later
  lua_getglobal(LuaState, "root");

  ImpLog(LL_Info, LC_Profile, "Lua profile execute success\n");
}

void ClearProfile() { ClearProfileInternal(); }

}  // namespace Profile
}  // namespace Impacto
