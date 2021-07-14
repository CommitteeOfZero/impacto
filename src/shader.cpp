#include <algorithm>

#include "shader.h"

#include "impacto.h"
#include "io/io.h"
#include "log.h"
#include "3d/model.h"
#include "util.h"
#include "window.h"

namespace Impacto {

static char const ShaderPath[] = "./shaders";
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

int PrintParameter(char* dest, int destSz, char const* name,
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
      ImpLog(LL_Error, LC_Render, "Invalid shader parameter type %d\n",
             param.Type);
      if (destSz > 0) *dest = '\0';
      return 0;
  }
}

GLuint ShaderAttach(GLuint program, GLenum shaderType, char const* path,
                    char const* params) {
  ImpLog(LL_Debug, LC_Render, "Loading shader object (type %d) \"%s\"\n",
         shaderType, path);

  size_t sourceRawSz;
  char* source = (char*)SDL_LoadFile(path, &sourceRawSz);
  if (!source) {
    ImpLog(LL_Debug, LC_Render, "Failed to read shader source file\n");
    return 0;
  }

  GLuint shader = glCreateShader(shaderType);
  if (!shader) {
    ImpLog(LL_Fatal, LC_Render, "Failed to create shader object\n");
    SDL_free(source);
    return 0;
  }

  const GLchar* codeParts[4];
  codeParts[0] = (Window::ActualGraphicsApi != Window::GfxApi_GL)
                     ? ShaderHeaderES
                     : ShaderHeader;
  codeParts[1] =
      shaderType == GL_VERTEX_SHADER ? ShaderHeaderVert : ShaderHeaderFrag;
  codeParts[2] = params;
  codeParts[3] = source;

  GLint codeLengths[4];
  codeLengths[0] = (Window::ActualGraphicsApi != Window::GfxApi_GL)
                       ? ShaderHeaderESLength
                       : ShaderHeaderLength;
  codeLengths[1] = shaderType == GL_VERTEX_SHADER ? ShaderHeaderVertLength
                                                  : ShaderHeaderFragLength;
  codeLengths[2] = strlen(params);
  codeLengths[3] = strlen(source);

  glShaderSource(shader, 4, codeParts, codeLengths);

  GLint result = 0;
  static GLchar errorLog[1024] = {0};

  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (!result) {
    glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
    ImpLog(LL_Fatal, LC_Render, "Error compiling shader: %s\n", errorLog);
    SDL_free(source);
    glDeleteShader(shader);
    return 0;
  }

  glAttachShader(program, shader);

  SDL_free(source);

  return shader;
}

GLuint ShaderCompile(char const* name, ShaderParamMap const& params) {
  GLuint program = glCreateProgram();
  if (!program) {
    ImpLog(LL_Fatal, LC_Render, "Could not create shader program\n");
    return program;
  }

  ImpLog(LL_Debug, LC_Render, "Compiling shader \"%s\"\n", name);

  size_t pathSz =
      std::max(
          snprintf(NULL, 0, "%s/%s%s", ShaderPath, name, FragShaderExtension),
          snprintf(NULL, 0, "%s/%s%s", ShaderPath, name, VertShaderExtension)) +
      1;

  char* fullPath = (char*)ImpStackAlloc(pathSz);

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

  sprintf(fullPath, "%s/%s%s", ShaderPath, name, VertShaderExtension);
  GLuint vs = ShaderAttach(program, GL_VERTEX_SHADER, fullPath, paramStr);
  if (!vs) {
    glDeleteProgram(program);
    ImpStackFree(paramStr);
    ImpStackFree(fullPath);
    return 0;
  }
  sprintf(fullPath, "%s/%s%s", ShaderPath, name, FragShaderExtension);
  GLuint fs = ShaderAttach(program, GL_FRAGMENT_SHADER, fullPath, paramStr);
  if (!fs) {
    static GLchar errorLog[1024] = {};
    glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
    ImpLog(LL_Fatal, LC_Render, "Error linking shader program: %s\n", errorLog);
    glDeleteShader(vs);
    glDeleteProgram(program);
    ImpStackFree(paramStr);
    ImpStackFree(fullPath);
    return 0;
  }

  ImpStackFree(paramStr);
  ImpStackFree(fullPath);

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
    ImpLog(LL_Fatal, LC_Render, "Error linking shader program: %s\n", errorLog);
    glDeleteProgram(program);
    return 0;
  }

  // TODO: Figure out why this actually doesn't work on macOS
#ifndef __APPLE__
  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
    ImpLog(LL_Fatal, LC_Render, "Shader program failed to validate: %s\n",
           errorLog);
    glDeleteProgram(program);
    return 0;
  }
#endif

  return program;
}

}  // namespace Impacto