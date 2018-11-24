#pragma once

#include <rapidjson/document.h>
#include <initializer_list>
#include <glm/glm.hpp>

using namespace rapidjson;

namespace Impacto {
namespace Profile {

extern Document Json;
extern Value GlobalNull;

void LoadJsonString(char const* str);

Value const& AssertIs(Value const& val, char const* path, Type type);
Value const& AssertIsOneOf(Value const& val, char const* path,
                           std::initializer_list<Type> types);

Value const& EnsureGetMember(Value const& val, char const* path,
                             char const* member);
int EnsureGetInt(Value const& val, char const* path);
float EnsureGetFloat(Value const& val, char const* path);
char const* EnsureGetString(Value const& val, char const* path);
glm::vec3 EnsureGetVec3(Value const& val, char const* path);

}  // namespace Profile
}  // namespace Impacto