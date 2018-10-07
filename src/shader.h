#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../vendor/include/flat_hash_map.hpp"

namespace Impacto {

enum ShaderParameterType {
  SPT_Int,
  SPT_Float,

  SPT_Vec2,
  SPT_Vec3,
  SPT_Vec4,

  SPT_Ivec2,
  SPT_Ivec3,
  SPT_Ivec4
};

struct ShaderParameter {
  ShaderParameterType Type;
  union {
    int Val_Int;
    float Val_Float;

    glm::vec2 Val_Vec2;
    glm::vec3 Val_Vec3;
    glm::vec4 Val_Vec4;

    glm::ivec2 Val_Ivec2;
    glm::ivec3 Val_Ivec3;
    glm::ivec4 Val_Ivec4;
  };
};

typedef ska::flat_hash_map<std::string, ShaderParameter> ShaderParamMap;

GLuint ShaderCompile(char const* name,
                     ShaderParamMap const& params = ShaderParamMap());

}  // namespace Impacto