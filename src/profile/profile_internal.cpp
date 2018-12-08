#include "profile_internal.h"

#include <stack>

#include "../log.h"
#include "../window.h"

#include <rapidjson/error/en.h>

#include "sprites.h"
#include "fonts.h"
#include "animations.h"

namespace Impacto {
namespace Profile {

Document Json;
Value GlobalNull;

static char const* kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                   "Array", "String", "Number"};

struct JsonStackItem {
  JsonStackItem(Value const& val, char* pathEnd) : Val(val), PathEnd(pathEnd) {}

  Value const& Val;
  char* PathEnd;
};

std::stack<JsonStackItem> JsonStack;
char JsonStackPath[1024] = {0};

void PushValue(Value const& val, char const* name) {
  ImpLogSlow(LL_Trace, LC_Profile, "PUSH %s\n", name);

  assert(!JsonStack.empty());
  char* pathEnd = JsonStack.top().PathEnd;
  size_t nameLength = strlen(name);
  assert((pathEnd + 1 + nameLength) - JsonStackPath < 1024);
  *pathEnd = '/';
  pathEnd++;
  memcpy(pathEnd, name, strlen(name));
  pathEnd += strlen(name);
  *pathEnd = '\0';
  JsonStack.emplace(val, pathEnd);
}

void PushValue(Value const& val, uint32_t name) {
  char nameStr[11];
  SDL_uitoa(name, nameStr, 10);
  PushValue(val, nameStr);
}

Value const& TopVal() {
  assert(!JsonStack.empty());
  return JsonStack.top().Val;
}

void PushArrayElement(uint32_t i) { PushValue(TopVal()[i], i); }

void Pop() {
  assert(!JsonStack.empty());
  JsonStack.pop();
  char* pathEnd;
  if (JsonStack.empty()) {
    pathEnd = JsonStackPath;
  } else {
    pathEnd = JsonStack.top().PathEnd;
  }

  ImpLogSlow(LL_Trace, LC_Profile, "POP %s\n", pathEnd + 1);

  *pathEnd = '\0';
}

bool TryPushMember(char const* name) {
  assert(!JsonStack.empty());
  Value const& topVal = TopVal();
  auto it = topVal.FindMember(name);
  if (it == topVal.MemberEnd()) {
    return false;
  }
  PushValue(it->value, name);
  return true;
}

void EnsurePushMember(char const* name) {
  bool success = TryPushMember(name);
  if (!success) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to have member %s\n",
           JsonStackPath, name);
    Window::Shutdown();
  }
}

void EnsurePushMemberOfType(char const* name, Type type) {
  EnsurePushMember(name);
  AssertIs(type);
}

void AssertIs(Type type) {
  Type actualType = TopVal().GetType();
  if (actualType != type) {
    ImpLog(LL_Fatal, LC_Profile,
           "Expected %s to have type %s, actual type %s\n", JsonStackPath,
           kTypeNames[type], kTypeNames[actualType]);
    Window::Shutdown();
  }
}

void AssertIsOneOf(std::initializer_list<Type> types) {
  Type actualType = TopVal().GetType();
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
         "Expected %s to have type in (%s), actual type %s\n", JsonStackPath,
         typeList, kTypeNames[actualType]);
  Window::Shutdown();
}

void PushMemberIterator(Value::ConstMemberIterator it) {
  PushValue(it->value, it->name.GetString());
}

void EnsurePushMemberIteratorOfType(Value::ConstMemberIterator it, Type type) {
  PushMemberIterator(it);
  AssertIs(type);
}

#define JSON_GET_METHODS(typeName, nativeType, typeDesc)                     \
  nativeType EnsureGet##typeName() {                                         \
    nativeType result;                                                       \
    bool success = TryGet##typeName(result);                                 \
    if (!success) {                                                          \
      ImpLog(LL_Fatal, LC_Profile, "Expected %s to be " typeDesc "\n");      \
      Window::Shutdown();                                                    \
    }                                                                        \
    return result;                                                           \
  }                                                                          \
  bool TryGetMember##typeName(char const* name, nativeType& out##typeName) { \
    if (!TryPushMember(name)) return false;                                  \
    bool result = TryGet##typeName(out##typeName);                           \
    Pop();                                                                   \
    return result;                                                           \
  }                                                                          \
  nativeType EnsureGetMember##typeName(char const* name) {                   \
    EnsurePushMember(name);                                                  \
    nativeType result = EnsureGet##typeName();                               \
    Pop();                                                                   \
    return result;                                                           \
  }                                                                          \
  bool TryGetArrayElement##typeName(uint32_t index,                          \
                                    nativeType& out##typeName) {             \
    PushArrayElement(index);                                                 \
    bool result = TryGet##typeName(out##typeName);                           \
    Pop();                                                                   \
    return result;                                                           \
  }                                                                          \
  nativeType EnsureGetArrayElement##typeName(uint32_t index) {               \
    PushArrayElement(index);                                                 \
    nativeType result = EnsureGet##typeName();                               \
    Pop();                                                                   \
    return result;                                                           \
  }

#define TRY_GET_ENTITY(typeName, nativeType, arrayName)        \
  bool TryGet##typeName(nativeType& out##typeName) {           \
    Value const& val = TopVal();                               \
    char const* name##typeName;                                \
    if (!TryGetString(name##typeName)) return false;           \
                                                               \
    auto const ref##typeName = arrayName.find(name##typeName); \
    if (ref##typeName == arrayName.end()) return false;        \
                                                               \
    out##typeName = ref##typeName->second;                     \
    return true;                                               \
  }

bool TryGetBool(bool& outBool) {
  Value const& val = TopVal();
  if (val.IsBool()) {
    outBool = val.GetBool();
    return true;
  }
  // TODO conversion?

  return false;
}

JSON_GET_METHODS(Bool, bool, "boolean")

bool TryGetUint(uint32_t& outUint) {
  Value const& val = TopVal();
  if (val.IsUint()) {
    outUint = val.GetUint();
    return true;
  }
  if (val.IsString()) {
    char* endp;
    outUint = SDL_strtoul(val.GetString(), &endp, 10);
    if (endp != 0) return true;
  }

  return false;
}

uint32_t EnsureGetKeyUint(Value::ConstMemberIterator it) {
  PushValue(it->name, "(name)");
  uint32_t result = EnsureGetUint();
  Pop();
  return result;
}

JSON_GET_METHODS(Uint, uint32_t, "unsigned integer convertible")

bool TryGetInt(int32_t& outInt) {
  Value const& val = TopVal();
  if (val.IsInt()) {
    outInt = val.GetInt();
    return true;
  }
  if (val.IsString()) {
    char* endp;
    outInt = SDL_strtol(val.GetString(), &endp, 10);
    if (endp != 0) return true;
  }

  return false;
}

int32_t EnsureGetKeyInt(Value::ConstMemberIterator it) {
  PushValue(it->name, "(name)");
  int32_t result = EnsureGetInt();
  Pop();
  return result;
}

JSON_GET_METHODS(Int, int32_t, "signed integer convertible")

bool TryGetFloat(float& outFloat) {
  Value const& val = TopVal();
  if (val.IsNumber()) {
    outFloat = val.GetFloat();
    return true;
  }
  if (val.IsString()) {
    char* endp;
    outFloat = SDL_strtod(val.GetString(), &endp);
    if (endp != 0) return true;
  }

  return false;
}

JSON_GET_METHODS(Float, float, "float convertible")

bool TryGetString(char const*& outString) {
  Value const& val = TopVal();
  if (!val.IsString()) return false;
  outString = val.GetString();
  return true;
}

char const* EnsureGetKeyString(Value::ConstMemberIterator it) {
  // All object keys are parsed as strings
  return it->name.GetString();
}

JSON_GET_METHODS(String, char const*, "string")

bool TryGetVec2(glm::vec2& outVec2) {
  Value const& val = TopVal();
  if (!val.IsObject()) return false;

  return TryGetMemberFloat("X", outVec2.x) && TryGetMemberFloat("Y", outVec2.y);
}

JSON_GET_METHODS(Vec2, glm::vec2, "Vec2")

bool TryGetVec3(glm::vec3& outVec3) {
  Value const& val = TopVal();
  if (!val.IsObject()) return false;

  return TryGetMemberFloat("X", outVec3.x) &&
         TryGetMemberFloat("Y", outVec3.y) && TryGetMemberFloat("Z", outVec3.z);
}

JSON_GET_METHODS(Vec3, glm::vec3, "Vec3")

bool TryGetRectF(RectF& outRectF) {
  Value const& val = TopVal();
  if (!val.IsObject()) return false;

  return TryGetMemberFloat("X", outRectF.X) &&
         TryGetMemberFloat("Y", outRectF.Y) &&
         TryGetMemberFloat("Width", outRectF.Width) &&
         TryGetMemberFloat("Height", outRectF.Height);
}

JSON_GET_METHODS(RectF, RectF, "RectF")

bool TryGetAssetPath(Io::AssetPath& outPath) {
  Value const& val = TopVal();
  if (val.IsString()) {
    outPath.Mount = "";
    outPath.Id = 0;
    outPath.FileName = val.GetString();
    return true;
  } else {
    if (!val.IsObject()) return false;

    if (!TryGetMemberUint("Id", outPath.Id)) return false;
    char const* _mount;
    if (!TryGetMemberString("Mount", _mount)) return false;
    outPath.Mount = _mount;
    return true;
  }
}

JSON_GET_METHODS(AssetPath, Io::AssetPath, "AssetPath")

TRY_GET_ENTITY(Sprite, Sprite, Sprites)
JSON_GET_METHODS(Sprite, Sprite, "Sprite")

TRY_GET_ENTITY(SpriteSheet, SpriteSheet, SpriteSheets)
JSON_GET_METHODS(SpriteSheet, SpriteSheet, "SpriteSheet")

TRY_GET_ENTITY(Font, Font*, Fonts)
JSON_GET_METHODS(Font, Font*, "Font")

TRY_GET_ENTITY(Animation, SpriteAnimationDef, Animations)
JSON_GET_METHODS(Animation, SpriteAnimationDef, "Animation")

void LoadJsonString(char const* str) {
  GlobalNull.SetNull();

  Json.Parse(str);

  if (Json.HasParseError()) {
    ImpLog(LL_Fatal, LC_Profile,
           "Failed to parse JSON from profile (at %d): %s\n",
           Json.GetErrorOffset(), GetParseError_En(Json.GetParseError()));
    Window::Shutdown();
  }

  JsonStackPath[0] = '\0';
  JsonStack.emplace(Json, JsonStackPath);
}

void ClearJsonProfileInternal() {
  while (!JsonStack.empty()) Pop();
  JsonStackPath[0] = '\0';
  Json.SetNull();
}

}  // namespace Profile
}  // namespace Impacto