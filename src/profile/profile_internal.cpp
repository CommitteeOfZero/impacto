#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

#include <rapidjson/error/en.h>

namespace Impacto {
namespace Profile {

Document Json;
Value GlobalNull;

static char const* kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                   "Array", "String", "Number"};

void LoadJsonString(char const* str) {
  GlobalNull.SetNull();

  Json.Parse(str);

  if (Json.HasParseError()) {
    ImpLog(LL_Fatal, LC_Profile,
           "Failed to parse JSON from profile (at %d): %s\n",
           Json.GetErrorOffset(), GetParseError_En(Json.GetParseError()));
    Window::Shutdown();
  }
}

Value const& AssertIs(Value const& val, char const* path, Type type) {
  Type actualType = val.GetType();
  if (actualType == type) return val;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to have type %s, actual type %s\n",
         path, kTypeNames[type], kTypeNames[actualType]);
  Window::Shutdown();

  return GlobalNull;
}

Value const& AssertIsOneOf(Value const& val, char const* path,
                           std::initializer_list<Type> types) {
  Type actualType = val.GetType();
  for (auto type : types) {
    if (actualType == type) return val;
  }

  char typeList[80];  // should be enough given type name lengths
  memset(typeList, 0, sizeof(typeList));
  bool first = true;
  for (auto type : types) {
    if (!first) strcat(typeList, ", ");
    first = false;
    strcat(typeList, kTypeNames[type]);
  }

  ImpLog(LL_Fatal, LC_Profile,
         "Expected %s to have type in (%s), actual type %s\n", path, typeList,
         kTypeNames[actualType]);
  Window::Shutdown();

  return GlobalNull;
}

Value const& EnsureGetMember(Value const& val, char const* path,
                             char const* member) {
  auto it = val.FindMember(member);
  if (it != val.MemberEnd()) return it->value;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to have member %s\n", path, member);
  Window::Shutdown();

  return GlobalNull;
}

int EnsureGetInt(Value const& val, char const* path) {
  if (val.IsInt()) return val.GetInt();
  if (val.IsString()) {
    char* endp;
    int result = SDL_strtol(val.GetString(), &endp, 10);
    if (endp != 0) return result;
  }

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be integer convertible\n", path);
  Window::Shutdown();

  return 0;
}

float EnsureGetFloat(Value const& val, char const* path) {
  if (val.IsNumber()) return val.GetFloat();
  if (val.IsString()) {
    char* endp;
    double result = SDL_strtod(val.GetString(), &endp);
    if (endp != 0) return result;
  }

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be float convertible\n", path);
  Window::Shutdown();

  return 0;
}

char const* EnsureGetString(Value const& val, char const* path) {
  if (val.IsString()) return val.GetString();

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be string\n", path);
  Window::Shutdown();

  return 0;
}

glm::vec3 EnsureGetVec3(Value const& val, char const* path) {
  AssertIs(val, path, kObjectType);

  int pathLen = strlen(path);

  char* pathBuf = (char*)ImpStackAlloc(pathLen + 4);
  strcpy(pathBuf, path);

  glm::vec3 result;
  strcpy(pathBuf + pathLen, "/X");
  result.x = EnsureGetFloat(EnsureGetMember(val, path, "X"), pathBuf);
  strcpy(pathBuf + pathLen, "/Y");
  result.y = EnsureGetFloat(EnsureGetMember(val, path, "Y"), pathBuf);
  strcpy(pathBuf + pathLen, "/Z");
  result.z = EnsureGetFloat(EnsureGetMember(val, path, "Z"), pathBuf);

  ImpStackFree(pathBuf);

  return result;
}

}  // namespace Profile
}  // namespace Impacto