#include "profile.h"
#include "profile_internal.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../window.h"
#include <flat_hash_map.hpp>

#include <duktape.h>

#include "../text.h"
#include "../game.h"
#include "../spritesheet.h"

#include "game.h"

namespace Impacto {
namespace Profile {

static ska::flat_hash_set<std::string> IncludedFiles;

static void DukFatal(void* udata, char const* msg) {
  ImpLog(LL_Fatal, LC_Profile, "JavaScript fatal error: %s\n",
         msg ? msg : "no message");
  Window::Shutdown();
}

static duk_ret_t DukPrint(duk_context* ctx) {
  ImpLog(LL_Info, LC_Profile, "JS: %s\n", duk_safe_to_string(ctx, 0));
  return 0;
}

static duk_ret_t DukInclude(duk_context* ctx) {
  std::string file = "profiles/" + std::string(duk_safe_to_string(ctx, 0));
  if (IncludedFiles.find(file) != IncludedFiles.end()) {
    ImpLog(LL_Debug, LC_Profile, "File %s already included, skipping...\n",
           file.c_str());
    return 0;
  }

  IncludedFiles.insert(file);
  ImpLog(LL_Debug, LC_Profile, "Including %s\n", file.c_str());

  Io::InputStream* stream;
  IoError err = Io::PhysicalFileStream::Create(file, &stream);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_Profile, "Could not open %s\n", file.c_str());
    return DUK_RET_ERROR;
  }

  char const prefix[] = "(function() {";
  char const suffix[] = "})();";

  char* script =
      (char*)malloc(strlen(prefix) + stream->Meta.Size + strlen(suffix));
  memcpy(script, prefix, strlen(prefix));

  int64_t len = stream->Read(script + strlen(prefix), stream->Meta.Size);

  if (len < 0) {
    free(script);
    delete stream;

    ImpLog(LL_Error, LC_Profile, "Could not open %s\n", file.c_str());
    return DUK_RET_ERROR;
  }

  len += strlen(prefix);
  memcpy(script + len, suffix, strlen(suffix));
  len += strlen(suffix);

  int evalErr = duk_peval_lstring(ctx, script, len);

  free(script);
  delete stream;

  if (evalErr != 0) {
    ImpLog(LL_Error, LC_Profile, "JS include('%s') error: %s\n", file.c_str(),
           duk_safe_to_string(ctx, -1));
    duk_pop(ctx);
    return DUK_RET_EVAL_ERROR;
  } else {
    ImpLog(LL_Debug, LC_Profile, "JS include('%s') success\n", file.c_str());
    duk_pop(ctx);
    return 0;
  }
}

template <typename Enum>
static void DefineEnumDouble(duk_context* ctx) {
  duk_idx_t obj_idx = duk_push_object(ctx);
  for (Enum value : Enum::_values()) {
    duk_push_number(ctx, value._value);
    duk_put_prop_string(ctx, obj_idx, value._to_string());
  }
  duk_put_global_string(ctx, Enum::_name());
}

template <typename Enum>
static void DefineEnumInt(duk_context* ctx) {
  duk_idx_t obj_idx = duk_push_object(ctx);
  for (Enum value : Enum::_values()) {
    duk_push_int(ctx, value._value);
    duk_put_prop_string(ctx, obj_idx, value._to_string());
  }
  duk_put_global_string(ctx, Enum::_name());
}

template <typename Enum>
static void DefineEnumUint(duk_context* ctx) {
  duk_idx_t obj_idx = duk_push_object(ctx);
  for (Enum value : Enum::_values()) {
    duk_push_uint(ctx, value._value);
    duk_put_prop_string(ctx, obj_idx, value._to_string());
  }
  duk_put_global_string(ctx, Enum::_name());
}

void MakeJsonProfile(std::string const& name) {
  Io::InputStream* stream;
  IoError err =
      Io::PhysicalFileStream::Create("profiles/" + name + "/game.js", &stream);
  if (err != IoError_OK) {
    ImpLog(LL_Fatal, LC_Profile, "Could not open profiles/%s/game.js\n",
           name.c_str());
    Window::Shutdown();
  }

  char* script = (char*)malloc(stream->Meta.Size);
  int64_t len = stream->Read(script, stream->Meta.Size);
  if (len < 0) {
    ImpLog(LL_Fatal, LC_Profile, "Could not open profiles/%s/game.js\n",
           name.c_str());
    Window::Shutdown();
  }

  duk_context* ctx = duk_create_heap(NULL, NULL, NULL, NULL, DukFatal);

  // Set up API
  duk_push_c_function(ctx, DukPrint, 1);
  duk_put_global_string(ctx, "print");
  duk_push_c_function(ctx, DukInclude, 1);
  duk_put_global_string(ctx, "include");

  // Root profile object
  duk_push_object(ctx);
  duk_put_global_string(ctx, "root");

  // Enums /sigh
  DefineEnumInt<TextAlignment>(ctx);
  DefineEnumInt<GameFeature>(ctx);
  DefineEnumInt<CharacterTypeFlags>(ctx);

  ImpLog(LL_Info, LC_Profile, "Starting profile %s\n", name.c_str());

  int evalErr = duk_peval_lstring(ctx, script, len);

  free(script);
  delete stream;

  if (evalErr != 0) {
    ImpLog(LL_Fatal, LC_Profile, "JS profile execute error: %s\n",
           duk_safe_to_string(ctx, -1));
    Window::Shutdown();
  }

  ImpLog(LL_Info, LC_Profile, "JS profile execute success\n");
  duk_get_global_string(ctx, "root");

  char const* jsonStr = duk_json_encode(ctx, -1);

  LoadJsonString(jsonStr);

  duk_destroy_heap(ctx);
}

void ClearJsonProfile() { Json.SetNull(); }

}  // namespace Profile
}  // namespace Impacto