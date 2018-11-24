#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

#include <rapidjson/error/en.h>

namespace Impacto {
namespace Profile {

Document Json;

static char const* kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                   "Array", "String", "Number"};

void LoadJsonString(char const* str) {
  Json.Parse(str);

  if (Json.HasParseError()) {
    ImpLog(LL_Fatal, LC_Profile,
           "Failed to parse JSON from profile (at %d): %s\n",
           Json.GetErrorOffset(), GetParseError_En(Json.GetParseError()));
    Window::Shutdown();
  }
}

void AssertIs(Value const& val, char const* path, Type type) {
  Type actualType = val.GetType();
  if (actualType == type) return;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to have type %s, actual type %s\n",
         path, kTypeNames[type], kTypeNames[actualType]);
  Window::Shutdown();
}

void AssertIsOneOf(Value const& val, char const* path,
                   std::initializer_list<Type> types) {
  Type actualType = val.GetType();
  for (auto type : types) {
    if (actualType == type) return;
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
}

void AssertIsIntegral(Value const& val, char const* path) {
  if (val.IsInt()) return;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be integral\n", path);
  Window::Shutdown();
}

void AssertHasMember(Value const& val, char const* path, char const* member) {
  if (val.HasMember(member)) return;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to have member %s\n", path, member);
  Window::Shutdown();
}

}  // namespace Profile
}  // namespace Impacto