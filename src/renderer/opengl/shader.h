#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <flat_hash_map.hpp>
#include <string>
#include "window.h"

namespace Impacto {
namespace OpenGL {

enum ShaderParameterType {
  SPT_Int,
  SPT_Float,

  SPT_MacroInt,
  SPT_MacroFloat,

  SPT_Vec2,
  SPT_Vec3,
  SPT_Vec4,

  SPT_Ivec2,
  SPT_Ivec3,
  SPT_Ivec4
};
constexpr inline int format_as(ShaderParameterType type) {
  return to_underlying(type);
}

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

  ShaderParameter() {}
  ShaderParameter(int val) : Type(SPT_Int), Val_Int(val) {}
  ShaderParameter(float val) : Type(SPT_Float), Val_Float(val) {}
  ShaderParameter(int val, bool macro)
      : Type(macro ? SPT_MacroInt : SPT_Int), Val_Int(val) {}
  ShaderParameter(float val, bool macro)
      : Type(macro ? SPT_MacroFloat : SPT_Float), Val_Float(val) {}
  ShaderParameter(glm::vec2 val) : Type(SPT_Vec2), Val_Vec2(val) {}
  ShaderParameter(glm::vec3 val) : Type(SPT_Vec3), Val_Vec3(val) {}
  ShaderParameter(glm::vec4 val) : Type(SPT_Vec4), Val_Vec4(val) {}
  ShaderParameter(glm::ivec2 val) : Type(SPT_Ivec2), Val_Ivec2(val) {}
  ShaderParameter(glm::ivec3 val) : Type(SPT_Ivec3), Val_Ivec3(val) {}
  ShaderParameter(glm::ivec4 val) : Type(SPT_Ivec4), Val_Ivec4(val) {}
};

typedef ska::flat_hash_map<std::string, ShaderParameter> ShaderParamMap;

class ShaderCompiler {
 public:
  GLuint Compile(char const* name,
                 ShaderParamMap const& params = ShaderParamMap());

 private:
  int PrintParameter(char* dest, int destSz, char const* name,
                     ShaderParameter const& param);
  GLuint Attach(GLuint program, GLenum shaderType, char const* path,
                char const* params);
};

}  // namespace OpenGL
}  // namespace Impacto