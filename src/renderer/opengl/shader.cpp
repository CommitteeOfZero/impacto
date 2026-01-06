#include <algorithm>

#include "shader.h"
#include "renderer.h"

#include "../../impacto.h"
#include "../../io/io.h"
#include "../../log.h"
#include "../3d/model.h"
#include "../../util.h"

namespace Impacto {
namespace OpenGL {

static char const ShaderPath[] = "./shaders/opengl";
static char const FragShaderExtension[] = "_frag.glsl";
static char const VertShaderExtension[] = "_vert.glsl";

// We need highp in vertex shaders, but uniforms shared between vertex and
// fragment shaders need to have the same precision in both

static char const ShaderHeader[] =
    "#version 330\n#define UNIFORM_PRECISION highp\n\n";
static GLint const ShaderHeaderLength =
    sizeof(ShaderHeader) - 1;  // without null terminator, for glShaderSource()
static char const ShaderHeaderES[] =
    "#version 300 es\n#define UNIFORM_PRECISION mediump\n\n";
static GLint const ShaderHeaderESLength = sizeof(ShaderHeaderES) - 1;
static char const ShaderHeaderVert[] =
    "#define VERTEX_SHADER\n#ifdef GL_ES\nprecision highp float;\n#endif\n\n";
static GLint const ShaderHeaderVertLength = sizeof(ShaderHeaderVert) - 1;
static char const ShaderHeaderFrag[] =
    "#define FRAGMENT_SHADER\n#ifdef GL_ES\nprecision mediump "
    "float;\n#endif\n\n";
static GLint const ShaderHeaderFragLength = sizeof(ShaderHeaderFrag) - 1;

int ShaderCompiler::PrintParameter(char* dest, int destSz, char const* name,
                                   ShaderParameter const& param) {
  switch (param.Type) {
    case SPT_Float:
      return snprintf(dest, destSz, "const float %s = %f;\n", name,
                      param.Val_Float);
      break;
    case SPT_Int:
      return snprintf(dest, destSz, "const int %s = %d;\n", name,
                      param.Val_Int);
      break;

    case SPT_MacroFloat:
      return snprintf(dest, destSz, "#define %s %f\n", name, param.Val_Float);
      break;
    case SPT_MacroInt:
      return snprintf(dest, destSz, "#define %s %d\n", name, param.Val_Int);
      break;

    case SPT_Vec2:
      return snprintf(dest, destSz, "const vec2 %s = vec2(%f, %f);\n", name,
                      param.Val_Vec2.x, param.Val_Vec2.y);
      break;
    case SPT_Vec3:
      return snprintf(dest, destSz, "const vec3 %s = vec3(%f, %f, %f);\n", name,
                      param.Val_Vec3.x, param.Val_Vec3.y, param.Val_Vec3.z);
      break;
    case SPT_Vec4:
      return snprintf(dest, destSz, "const vec4 %s = vec4(%f, %f, %f, %f);\n",
                      name, param.Val_Vec4.x, param.Val_Vec4.y,
                      param.Val_Vec4.z, param.Val_Vec4.w);
      break;

    case SPT_Ivec2:
      return snprintf(dest, destSz, "const ivec2 %s = ivec2(%d, %d);\n", name,
                      param.Val_Ivec2.x, param.Val_Ivec2.y);
      break;
    case SPT_Ivec3:
      return snprintf(dest, destSz, "const ivec3 %s = ivec3(%d, %d, %d);\n",
                      name, param.Val_Ivec3.x, param.Val_Ivec3.y,
                      param.Val_Ivec3.z);
      break;
    case SPT_Ivec4:
      return snprintf(dest, destSz, "const ivec4 %s = ivec4(%d, %d, %d, %d);\n",
                      name, param.Val_Ivec4.x, param.Val_Ivec4.y,
                      param.Val_Ivec4.z, param.Val_Ivec4.w);
      break;

    default:
      ImpLog(LogLevel::Error, LogChannel::Render,
             "Invalid shader parameter type {:d}\n", param.Type);
      if (destSz > 0) *dest = '\0';
      return 0;
  }
}

GLuint ShaderCompiler::Attach(GLuint program, GLenum shaderType,
                              char const* path, char const* params) {
  ImpLog(LogLevel::Debug, LogChannel::Render,
         "Loading shader object (type {:d}) \"{:s}\"\n", shaderType, path);

  size_t sourceRawSz;
  char* source = (char*)SDL_LoadFile(path, &sourceRawSz);
  if (!source) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to read shader source file\n");
    return 0;
  }

  GLuint shader = glCreateShader(shaderType);
  if (!shader) {
    ImpLog(LogLevel::Fatal, LogChannel::Render,
           "Failed to create shader object\n");
    SDL_free(source);
    return 0;
  }

  const GLchar* codeParts[4];
  codeParts[0] =
      (ActualGraphicsApi != GfxApi_GL) ? ShaderHeaderES : ShaderHeader;
  codeParts[1] =
      shaderType == GL_VERTEX_SHADER ? ShaderHeaderVert : ShaderHeaderFrag;
  codeParts[2] = params;
  codeParts[3] = source;

  GLint codeLengths[4];
  codeLengths[0] = (ActualGraphicsApi != GfxApi_GL) ? ShaderHeaderESLength
                                                    : ShaderHeaderLength;
  codeLengths[1] = shaderType == GL_VERTEX_SHADER ? ShaderHeaderVertLength
                                                  : ShaderHeaderFragLength;
  codeLengths[2] = (GLint)strlen(params);
  codeLengths[3] = (GLint)strlen(source);

  glShaderSource(shader, 4, codeParts, codeLengths);

  GLint result = 0;
  static GLchar errorLog[1024] = {0};

  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  assert(result);
  if (!result) {
    glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
    ImpLog(LogLevel::Fatal, LogChannel::Render,
           "Error compiling shader: {:s}\n", errorLog);
    SDL_free(source);
    glDeleteShader(shader);
    return 0;
  }

  glAttachShader(program, shader);

  SDL_free(source);

  return shader;
}

GLuint ShaderCompiler::Compile(char const* name, ShaderParamMap const& params) {
  GLuint program = glCreateProgram();
  if (!program) {
    ImpLog(LogLevel::Fatal, LogChannel::Render,
           "Could not create shader program\n");
    return program;
  }

  ImpLog(LogLevel::Debug, LogChannel::Render, "Compiling shader \"{:s}\"\n",
         name);

  int paramSz = 1;
  for (auto const& param : params) {
    paramSz += PrintParameter(0, 0, param.first.c_str(), param.second);
  }

  char* paramStr = (char*)ImpStackAlloc(paramSz);
  char* paramWrite = paramStr;
  int bytesLeft = paramSz;
  for (auto const& param : params) {
    int thisParamSz = PrintParameter(paramWrite, bytesLeft, param.first.c_str(),
                                     param.second);
    bytesLeft -= thisParamSz;
    paramWrite += thisParamSz;
  }
  paramStr[paramSz - 1] = '\0';

  std::string vertexShaderPath = fmt::format(FMT_COMPILE("{}/{}{}"), ShaderPath,
                                             name, VertShaderExtension);
  GLuint vs =
      Attach(program, GL_VERTEX_SHADER, vertexShaderPath.c_str(), paramStr);
  if (!vs) {
    glDeleteProgram(program);
    ImpStackFree(paramStr);
    return 0;
  }
  std::string fragShaderPath = fmt::format(FMT_COMPILE("{}/{}{}"), ShaderPath,
                                           name, FragShaderExtension);
  GLuint fs =
      Attach(program, GL_FRAGMENT_SHADER, fragShaderPath.c_str(), paramStr);
  if (!fs) {
    static GLchar errorLog[1024] = {};
    glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
    ImpLog(LogLevel::Fatal, LogChannel::Render,
           "Error linking shader program: {:s}\n", errorLog);
    glDeleteShader(vs);
    glDeleteProgram(program);
    ImpStackFree(paramStr);
    return 0;
  }

  ImpStackFree(paramStr);

  GLint result = 0;
  static GLchar errorLog[1024] = {};

  glLinkProgram(program);

  glDetachShader(program, vs);
  glDetachShader(program, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);

  glGetProgramiv(program, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
    ImpLog(LogLevel::Fatal, LogChannel::Render,
           "Error linking shader program: {:s}\n", errorLog);
    glDeleteProgram(program);
    return 0;
  }

  // TODO: Figure out why this actually doesn't work on macOS
#ifndef __APPLE__
  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
    ImpLog(LogLevel::Fatal, LogChannel::Render,
           "ShaderCompiler program failed to validate: {:s}\n", errorLog);
    glDeleteProgram(program);
    return 0;
  }
#endif

  return program;
}

SpriteShader::SpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      TransformationLocation(glGetUniformLocation(programId, "Transformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      ColorShiftLocation(glGetUniformLocation(programId, "ColorShift")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.Transformation, TransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.ColorShift, ColorShiftLocation);
}

void SpriteShader::UploadUniforms(SpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.Transformation, Uniforms.Transformation,
            TransformationLocation);
  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.ColorShift, Uniforms.ColorShift, ColorShiftLocation);
}

SpriteInvertedShader::SpriteInvertedShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      TransformationLocation(glGetUniformLocation(programId, "Transformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.Transformation, TransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
}

void SpriteInvertedShader::UploadUniforms(SpriteInvertedUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.Transformation, Uniforms.Transformation,
            TransformationLocation);
  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
}

YUVFrameShader::YUVFrameShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      LumaLocation(glGetUniformLocation(programId, "Luma")),
      CbLocation(glGetUniformLocation(programId, "Cb")),
      CrLocation(glGetUniformLocation(programId, "Cr")),
      IsAlphaLocation(glGetUniformLocation(programId, "IsAlpha")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.Luma, LumaLocation);
  UploadVar(Uniforms.Cb, CbLocation);
  UploadVar(Uniforms.Cr, CrLocation);
  UploadVar(Uniforms.IsAlpha, IsAlphaLocation);
}

void YUVFrameShader::UploadUniforms(YUVFrameUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);

  UpdateVar(newUniforms.Luma, Uniforms.Luma, LumaLocation);
  UpdateVar(newUniforms.Cb, Uniforms.Cb, CbLocation);
  UpdateVar(newUniforms.Cr, Uniforms.Cr, CrLocation);
  UpdateVar(newUniforms.IsAlpha, Uniforms.IsAlpha, IsAlphaLocation);
}

MaskedSpriteNoAlphaShader::MaskedSpriteNoAlphaShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")),
      AlphaLocation(glGetUniformLocation(programId, "Alpha")),
      IsInvertedLocation(glGetUniformLocation(programId, "IsInverted")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
  UploadVar(Uniforms.Alpha, AlphaLocation);
  UploadVar(Uniforms.IsInverted, IsInvertedLocation);
}

void MaskedSpriteNoAlphaShader::UploadUniforms(
    MaskedSpriteNoAlphaUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
  UpdateVar(newUniforms.Alpha, Uniforms.Alpha, AlphaLocation);
  UpdateVar(newUniforms.IsInverted, Uniforms.IsInverted, IsInvertedLocation);
}

MaskedSpriteShader::MaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")),
      AlphaLocation(glGetUniformLocation(programId, "Alpha")),
      IsInvertedLocation(glGetUniformLocation(programId, "IsInverted")),
      IsSameTextureLocation(glGetUniformLocation(programId, "IsSameTexture")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
  UploadVar(Uniforms.Alpha, AlphaLocation);
  UploadVar(Uniforms.IsInverted, IsInvertedLocation);
  UploadVar(Uniforms.IsSameTexture, IsSameTextureLocation);
}

void MaskedSpriteBinaryShader::UploadUniforms(
    MaskedSpriteBinaryUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);
  UpdateVar(newUniforms.FullscreenMask, Uniforms.FullscreenMask,
            FullscreenMaskLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
  UpdateVar(newUniforms.IsInverted, Uniforms.IsInverted, IsInvertedLocation);
}

MaskedSpriteBinaryShader::MaskedSpriteBinaryShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      FullscreenMaskLocation(glGetUniformLocation(programId, "FullscreenMask")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")),
      IsInvertedLocation(glGetUniformLocation(programId, "IsInverted")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.FullscreenMask, FullscreenMaskLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
  UploadVar(Uniforms.IsInverted, IsInvertedLocation);
}

void MaskedSpriteShader::UploadUniforms(MaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
  UpdateVar(newUniforms.Alpha, Uniforms.Alpha, AlphaLocation);
  UpdateVar(newUniforms.IsInverted, Uniforms.IsInverted, IsInvertedLocation);
  UpdateVar(newUniforms.IsSameTexture, Uniforms.IsSameTexture,
            IsSameTextureLocation);
}

ColorBurnMaskedSpriteShader::ColorBurnMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void ColorBurnMaskedSpriteShader::UploadUniforms(
    ColorBurnMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

ColorDodgeMaskedSpriteShader::ColorDodgeMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void ColorDodgeMaskedSpriteShader::UploadUniforms(
    ColorDodgeMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

HardLightMaskedSpriteShader::HardLightMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void HardLightMaskedSpriteShader::UploadUniforms(
    HardLightMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

LinearBurnMaskedSpriteShader::LinearBurnMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void LinearBurnMaskedSpriteShader::UploadUniforms(
    LinearBurnMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

OverlayMaskedSpriteShader::OverlayMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void OverlayMaskedSpriteShader::UploadUniforms(
    OverlayMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

ScreenMaskedSpriteShader::ScreenMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void ScreenMaskedSpriteShader::UploadUniforms(
    ScreenMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

SoftLightMaskedSpriteShader::SoftLightMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void SoftLightMaskedSpriteShader::UploadUniforms(
    SoftLightMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

ColorMaskedSpriteShader::ColorMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void ColorMaskedSpriteShader::UploadUniforms(
    ColorMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

AdditiveMaskedSpriteShader::AdditiveMaskedSpriteShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      SpriteTransformationLocation(
          glGetUniformLocation(programId, "SpriteTransformation")),
      MaskTransformationLocation(
          glGetUniformLocation(programId, "MaskTransformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.SpriteTransformation, SpriteTransformationLocation);
  UploadVar(Uniforms.MaskTransformation, MaskTransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
}

void AdditiveMaskedSpriteShader::UploadUniforms(
    AdditiveMaskedSpriteUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.SpriteTransformation, Uniforms.SpriteTransformation,
            SpriteTransformationLocation);
  UpdateVar(newUniforms.MaskTransformation, Uniforms.MaskTransformation,
            MaskTransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
}

CCMessageBoxShader::CCMessageBoxShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")),
      AlphaLocation(glGetUniformLocation(programId, "Alpha")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
  UploadVar(Uniforms.Alpha, AlphaLocation);
}

void CCMessageBoxShader::UploadUniforms(CCMessageBoxUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
  UpdateVar(newUniforms.Alpha, Uniforms.Alpha, AlphaLocation);
}

CHLCCMenuBackgroundShader::CHLCCMenuBackgroundShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      MaskLocation(glGetUniformLocation(programId, "Mask")),
      AlphaLocation(glGetUniformLocation(programId, "Alpha")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.Mask, MaskLocation);
  UploadVar(Uniforms.Alpha, AlphaLocation);
}

void CHLCCMenuBackgroundShader::UploadUniforms(
    CHLCCMenuBackgroundUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.Mask, Uniforms.Mask, MaskLocation);
  UpdateVar(newUniforms.Alpha, Uniforms.Alpha, AlphaLocation);
}

GaussianBlurShader::GaussianBlurShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      TransformationLocation(glGetUniformLocation(programId, "Transformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      TextureDimensionsLocation(
          glGetUniformLocation(programId, "TextureDimensions")),
      IsHorizontalLocation(glGetUniformLocation(programId, "IsHorizontal")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.Transformation, TransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.TextureDimensions, TextureDimensionsLocation);
  UploadVar(Uniforms.IsHorizontal, IsHorizontalLocation);
}

void GaussianBlurShader::UploadUniforms(GaussianBlurUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.Transformation, Uniforms.Transformation,
            TransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.TextureDimensions, Uniforms.TextureDimensions,
            TextureDimensionsLocation);
  UpdateVar(newUniforms.IsHorizontal, Uniforms.IsHorizontal,
            IsHorizontalLocation);
}

MosaicShader::MosaicShader(GLint programId)
    : Shader(programId),
      ProjectionLocation(glGetUniformLocation(programId, "Projection")),
      TransformationLocation(glGetUniformLocation(programId, "Transformation")),
      ColorMapLocation(glGetUniformLocation(programId, "ColorMap")),
      TextureDimensionsLocation(
          glGetUniformLocation(programId, "TextureDimensions")),
      TileSizeLocation(glGetUniformLocation(programId, "TileSize")) {
  UploadVar(Uniforms.Projection, ProjectionLocation);
  UploadVar(Uniforms.Transformation, TransformationLocation);
  UploadVar(Uniforms.ColorMap, ColorMapLocation);
  UploadVar(Uniforms.TextureDimensions, TextureDimensionsLocation);
  UploadVar(Uniforms.TileSize, TileSizeLocation);
}

void MosaicShader::UploadUniforms(MosaicUniforms newUniforms) {
  UpdateVar(newUniforms.Projection, Uniforms.Projection, ProjectionLocation);
  UpdateVar(newUniforms.Transformation, Uniforms.Transformation,
            TransformationLocation);

  UpdateVar(newUniforms.ColorMap, Uniforms.ColorMap, ColorMapLocation);
  UpdateVar(newUniforms.TextureDimensions, Uniforms.TextureDimensions,
            TextureDimensionsLocation);
  UpdateVar(newUniforms.TileSize, Uniforms.TileSize, TileSizeLocation);
}

}  // namespace OpenGL
}  // namespace Impacto
