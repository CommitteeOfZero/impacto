#pragma once

#include <rapidjson/document.h>
#include <initializer_list>
#include <glm/glm.hpp>
#include "../io/assetpath.h"
#include "../util.h"

#include "../spritesheet.h"
#include "../font.h"

using namespace rapidjson;

namespace Impacto {
namespace Profile {

extern Document Json;
extern Value GlobalNull;

void LoadJsonString(char const* str);

Value const& AssertIs(Value const& val, char const* path, Type type);
Value const& AssertIsOneOf(Value const& val, char const* path,
                           std::initializer_list<Type> types);

Value const& TryGetMember(Value const& val, char const* member,
                          bool& outSuccess);
Value const& EnsureGetMember(Value const& val, char const* path,
                             char const* member);
Value const& EnsureGetMemberOfType(Value const& val, char const* path,
                                   char const* member, Type type);
Value const& EnsureGetMemberOfTypes(Value const& val, char const* path,
                                    char const* member,
                                    std::initializer_list<Type> types);

// Yeah, these can't really be templated (except for TryGetMemberX)...
// TryGetX logic is type specific, EnsureGet(Member)X has different error
// strings for each type

bool TryGetString(Value const& val, char const*& outString);
bool TryGetMemberString(Value const& val, char const* member,
                        char const*& outString);
char const* EnsureGetString(Value const& val, char const* path);
char const* EnsureGetMemberString(Value const& val, char const* path,
                                  char const* member);
bool TryGetBool(Value const& val, bool& outBool);
bool TryGetMemberBool(Value const& val, char const* member, bool& outBool);
bool EnsureGetBool(Value const& val, char const* path);
bool EnsureGetMemberBool(Value const& val, char const* path,
                         char const* member);
bool TryGetInt(Value const& val, int& outInt);
bool TryGetMemberInt(Value const& val, char const* member, int& outInt);
int EnsureGetInt(Value const& val, char const* path);
int EnsureGetMemberInt(Value const& val, char const* path, char const* member);
bool TryGetUint(Value const& val, uint32_t& outUint);
bool TryGetMemberUint(Value const& val, char const* member, uint32_t& outUint);
uint32_t EnsureGetUint(Value const& val, char const* path);
uint32_t EnsureGetMemberUint(Value const& val, char const* path,
                             char const* member);
bool TryGetFloat(Value const& val, float& outFloat);
bool TryGetMemberFloat(Value const& val, char const* member, float& outFloat);
float EnsureGetFloat(Value const& val, char const* path);
float EnsureGetMemberFloat(Value const& val, char const* path,
                           char const* member);
bool TryGetVec2(Value const& val, glm::vec2& outVec);
bool TryGetMemberVec2(Value const& val, char const* member, glm::vec2& outVec);
glm::vec2 EnsureGetVec2(Value const& val, char const* path);
glm::vec2 EnsureGetMemberVec2(Value const& val, char const* path,
                              char const* member);
bool TryGetVec3(Value const& val, glm::vec3& outVec);
bool TryGetMemberVec3(Value const& val, char const* member, glm::vec3& outVec);
glm::vec3 EnsureGetVec3(Value const& val, char const* path);
glm::vec3 EnsureGetMemberVec3(Value const& val, char const* path,
                              char const* member);
bool TryGetRectF(Value const& val, RectF& outRect);
bool TryGetMemberRectF(Value const& val, char const* member, RectF& outRect);
RectF EnsureGetRectF(Value const& val, char const* path);
RectF EnsureGetMemberRectF(Value const& val, char const* path,
                           char const* member);
bool TryGetAssetPath(Value const& val, Io::AssetPath& outPath);
bool TryGetMemberAssetPath(Value const& val, char const* member,
                           Io::AssetPath& outPath);
Io::AssetPath EnsureGetAssetPath(Value const& val, char const* path);
Io::AssetPath EnsureGetMemberAssetPath(Value const& val, char const* path,
                                       char const* member);

Sprite EnsureGetMemberSprite(Value const& val, char const* path,
                             char const* member);
SpriteSheet EnsureGetMemberSpriteSheet(Value const& val, char const* path,
                                       char const* member);
Font EnsureGetMemberFont(Value const& val, char const* path,
                         char const* member);

}  // namespace Profile
}  // namespace Impacto