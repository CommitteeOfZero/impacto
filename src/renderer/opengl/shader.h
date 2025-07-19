#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ankerl/unordered_dense.h>
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

typedef ankerl::unordered_dense::map<std::string, ShaderParameter, string_hash,
                                     std::equal_to<>>
    ShaderParamMap;

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

template <typename UniformsStruct>
class Shader {
 public:
  Shader(GLuint programId) : ProgramId(programId) { Bind(); }

  ~Shader() { glDeleteProgram(ProgramId); }

  void Bind() { glUseProgram(ProgramId); }

  UniformsStruct GetUniforms() { return Uniforms; }
  virtual void UploadUniforms(UniformsStruct uniforms) = 0;

 protected:
  const GLuint ProgramId;

  UniformsStruct Uniforms{};

  void UploadVar(bool value, GLint location) { glUniform1i(location, value); }
  void UploadVar(int value, GLint location) { glUniform1i(location, value); }
  void UploadVar(GLuint value, GLint location) {
    glUniform1ui(location, value);
  }
  void UploadVar(float value, GLint location) { glUniform1f(location, value); }
  void UploadVar(glm::vec2 value, GLint location) {
    glUniform2fv(location, 1, &value[0]);
  }
  void UploadVar(glm::vec3 value, GLint location) {
    glUniform3fv(location, 1, &value[0]);
  }
  void UploadVar(glm::vec4 value, GLint location) {
    glUniform4fv(location, 1, &value[0]);
  }
  void UploadVar(glm::mat4 value, GLint location) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
  }

  template <typename T>
  void UpdateVar(T newValue, T& oldValue, GLint location) {
    if (newValue != oldValue) {
      oldValue = newValue;
      UploadVar(newValue, location);
    }
  }
};

struct AdditiveMaskedSpriteUniforms {
  bool operator==(const AdditiveMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class AdditiveMaskedSpriteShader : public Shader<AdditiveMaskedSpriteUniforms> {
 public:
  AdditiveMaskedSpriteShader(GLint programId);

  void UploadUniforms(AdditiveMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct SpriteUniforms {
  bool operator==(const SpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 Transformation{};

  GLint ColorMap = 0;
  glm::vec3 ColorShift{};
};

class SpriteShader : public Shader<SpriteUniforms> {
 public:
  SpriteShader(GLint programId);

  void UploadUniforms(SpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint TransformationLocation;

  const GLint ColorMapLocation;
  const GLint ColorShiftLocation;
};

struct SpriteInvertedUniforms {
  bool operator==(const SpriteInvertedUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 Transformation{};

  GLint ColorMap = 0;
};

class SpriteInvertedShader : public Shader<SpriteInvertedUniforms> {
 public:
  SpriteInvertedShader(GLint programId);

  void UploadUniforms(SpriteInvertedUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint TransformationLocation;

  const GLint ColorMapLocation;
};

struct YUVFrameUniforms {
  bool operator==(const YUVFrameUniforms& other) const = default;

  glm::mat4 Projection;

  GLint Luma = 0;
  GLint Cb = 0;
  GLint Cr = 0;
  bool IsAlpha = false;
};

class YUVFrameShader : public Shader<YUVFrameUniforms> {
 public:
  YUVFrameShader(GLint programId);

  void UploadUniforms(YUVFrameUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;

  const GLint LumaLocation;
  const GLint CbLocation;
  const GLint CrLocation;
  const GLint IsAlphaLocation;
};

struct MaskedSpriteNoAlphaUniforms {
  bool operator==(const MaskedSpriteNoAlphaUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap = 0;
  GLint Mask = 0;
  glm::vec2 Alpha{};
  bool IsInverted = false;
};

class MaskedSpriteNoAlphaShader : public Shader<MaskedSpriteNoAlphaUniforms> {
 public:
  MaskedSpriteNoAlphaShader(GLint programId);

  void UploadUniforms(MaskedSpriteNoAlphaUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
  const GLint AlphaLocation;
  const GLint IsInvertedLocation;
};

struct MaskedSpriteUniforms {
  bool operator==(const MaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
  glm::vec2 Alpha{};
  bool IsInverted = false;
  bool IsSameTexture = false;
};

class MaskedSpriteShader : public Shader<MaskedSpriteUniforms> {
 public:
  MaskedSpriteShader(GLint programId);

  void UploadUniforms(MaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
  const GLint AlphaLocation;
  const GLint IsInvertedLocation;
  const GLint IsSameTextureLocation;
};

struct ColorBurnMaskedSpriteUniforms {
  bool operator==(const ColorBurnMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class ColorBurnMaskedSpriteShader
    : public Shader<ColorBurnMaskedSpriteUniforms> {
 public:
  ColorBurnMaskedSpriteShader(GLint programId);

  void UploadUniforms(ColorBurnMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct ColorDodgeMaskedSpriteUniforms {
  bool operator==(const ColorDodgeMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class ColorDodgeMaskedSpriteShader
    : public Shader<ColorDodgeMaskedSpriteUniforms> {
 public:
  ColorDodgeMaskedSpriteShader(GLint programId);

  void UploadUniforms(ColorDodgeMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct ColorMaskedSpriteUniforms {
  bool operator==(const ColorMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class ColorMaskedSpriteShader : public Shader<ColorMaskedSpriteUniforms> {
 public:
  ColorMaskedSpriteShader(GLint programId);

  void UploadUniforms(ColorMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct HardLightMaskedSpriteUniforms {
  bool operator==(const HardLightMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class HardLightMaskedSpriteShader
    : public Shader<HardLightMaskedSpriteUniforms> {
 public:
  HardLightMaskedSpriteShader(GLint programId);

  void UploadUniforms(HardLightMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct LinearBurnMaskedSpriteUniforms {
  bool operator==(const LinearBurnMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class LinearBurnMaskedSpriteShader
    : public Shader<LinearBurnMaskedSpriteUniforms> {
 public:
  LinearBurnMaskedSpriteShader(GLint programId);

  void UploadUniforms(LinearBurnMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct OverlayMaskedSpriteUniforms {
  bool operator==(const OverlayMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class OverlayMaskedSpriteShader : public Shader<OverlayMaskedSpriteUniforms> {
 public:
  OverlayMaskedSpriteShader(GLint programId);

  void UploadUniforms(OverlayMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct ScreenMaskedSpriteUniforms {
  bool operator==(const ScreenMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class ScreenMaskedSpriteShader : public Shader<ScreenMaskedSpriteUniforms> {
 public:
  ScreenMaskedSpriteShader(GLint programId);

  void UploadUniforms(ScreenMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct SoftLightMaskedSpriteUniforms {
  bool operator==(const SoftLightMaskedSpriteUniforms& other) const = default;

  glm::mat4 Projection{};
  glm::mat4 SpriteTransformation{};
  glm::mat4 MaskTransformation{};

  GLint ColorMap;
  GLint Mask;
};

class SoftLightMaskedSpriteShader
    : public Shader<SoftLightMaskedSpriteUniforms> {
 public:
  SoftLightMaskedSpriteShader(GLint programId);

  void UploadUniforms(SoftLightMaskedSpriteUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;
  const GLint SpriteTransformationLocation;
  const GLint MaskTransformationLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
};

struct CCMessageBoxUniforms {
  bool operator==(const CCMessageBoxUniforms& other) const = default;

  glm::mat4 Projection{};

  GLint ColorMap;
  GLint Mask;
  glm::vec4 Alpha{};
};

class CCMessageBoxShader : public Shader<CCMessageBoxUniforms> {
 public:
  CCMessageBoxShader(GLint programId);

  void UploadUniforms(CCMessageBoxUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
  const GLint AlphaLocation;
};

struct CHLCCMenuBackgroundUniforms {
  bool operator==(const CHLCCMenuBackgroundUniforms& other) const = default;

  glm::mat4 Projection{};

  GLint ColorMap;
  GLint Mask;
  float Alpha = 0.0f;
};

class CHLCCMenuBackgroundShader : public Shader<CHLCCMenuBackgroundUniforms> {
 public:
  CHLCCMenuBackgroundShader(GLint programId);

  void UploadUniforms(CHLCCMenuBackgroundUniforms uniforms) override;

 private:
  const GLint ProjectionLocation;

  const GLint ColorMapLocation;
  const GLint MaskLocation;
  const GLint AlphaLocation;
};

}  // namespace OpenGL
}  // namespace Impacto