#pragma once

#include <rapidjson/document.h>
#include <initializer_list>

using namespace rapidjson;

namespace Impacto {
namespace Profile {

extern Document Json;

void LoadJsonString(char const* str);

void AssertIs(Value const& val, char const* path, Type type);
void AssertIsOneOf(Value const& val, char const* path,
                   std::initializer_list<Type> types);
void AssertIsIntegral(Value const& val, char const* path);
void AssertHasMember(Value const& val, char const* path, char const* member);

}  // namespace Profile
}  // namespace Impacto