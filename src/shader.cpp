#include <algorithm>

#include "shader.h"

#include "impacto.h"
#include "io/io.h"
#include "log.h"
#include "3d/model.h"
#include "util.h"

namespace Impacto {

static char const ShaderPath[] = "./shaders";
static char const FragShaderExtension[] = "_frag.glsl";
static char const VertShaderExtension[] = "_vert.glsl";

static char const ShaderHeaderTemplate[] =
    "\
#version 330                                                    \n\
                                                                \n\
const int ModelMaxBonesPerMesh = %d;                            \n\
                                                                \n\
";
static char* ShaderHeader;
GLint ShaderHeaderLength;  // without null terminator, for glShaderSource()

void ShaderInit() {
  assert(ShaderHeader == NULL);

  ImpLog(LL_Debug, LC_Render, "Initialising shader loader\n");

  size_t headerSz =
      snprintf(NULL, 0, ShaderHeaderTemplate, ModelMaxBonesPerMesh) + 1;
  ShaderHeader = (char*)malloc(headerSz);
  ShaderHeaderLength = headerSz - 1;
  sprintf(ShaderHeader, ShaderHeaderTemplate, ModelMaxBonesPerMesh);
}

bool ShaderAttach(GLuint program, GLenum shaderType, const char* path) {
  ImpLog(LL_Debug, LC_Render, "Loading shader object (type %d) \"%s\"\n",
         shaderType, path);

  size_t sourceRawSz;
  char* source = (char*)SDL_LoadFile(path, &sourceRawSz);
  if (!source) {
    ImpLog(LL_Debug, LC_Render, "Failed to read shader source file\n");
    return false;
  }

  GLuint shader = glCreateShader(shaderType);
  if (!shader) {
    ImpLog(LL_Fatal, LC_Render, "Failed to create shader object\n");
    SDL_free(source);
    return false;
  }

  const GLchar* codeParts[2];
  codeParts[0] = ShaderHeader;
  codeParts[1] = source;

  GLint codeLengths[2];
  codeLengths[0] = ShaderHeaderLength;
  codeLengths[1] = strlen(source);

  glShaderSource(shader, 2, codeParts, codeLengths);

  GLint result = 0;
  static GLchar errorLog[1024] = {0};

  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (!result) {
    glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
    ImpLog(LL_Fatal, LC_Render, "Error compiling shader: %s\n", errorLog);
    SDL_free(source);
    return false;
  }

  glAttachShader(program, shader);

  SDL_free(source);

  return true;
}

GLuint ShaderCompile(const char* path) {
  GLuint program = glCreateProgram();
  if (!program) {
    ImpLog(LL_Fatal, LC_Render, "Could not create shader program\n");
    return program;
  }

  ImpLog(LL_Debug, LC_Render, "Compiling shader \"%s\"\n", path);

  size_t pathSz =
      std::max(
          snprintf(NULL, 0, "%s/%s%s", ShaderPath, path, FragShaderExtension),
          snprintf(NULL, 0, "%s/%s%s", ShaderPath, path, VertShaderExtension)) +
      1;

  char* fullPath = (char*)ImpStackAlloc(pathSz);

  sprintf(fullPath, "%s/%s%s", ShaderPath, path, VertShaderExtension);
  if (!ShaderAttach(program, GL_VERTEX_SHADER, fullPath)) {
    glDeleteProgram(program);
    ImpStackFree(fullPath);
    return 0;
  }
  sprintf(fullPath, "%s/%s%s", ShaderPath, path, FragShaderExtension);
  if (!ShaderAttach(program, GL_FRAGMENT_SHADER, fullPath)) {
    glDeleteProgram(program);
    ImpStackFree(fullPath);
    return 0;
  }

  ImpStackFree(fullPath);

  GLint result = 0;
  static GLchar errorLog[1024] = {};

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
    ImpLog(LL_Fatal, LC_Render, "Error linking shader program: %s\n", errorLog);
    glDeleteProgram(program);
    return 0;
  }

  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
    ImpLog(LL_Fatal, LC_Render, "Shader program failed to validate: %s\n",
           errorLog);
    glDeleteProgram(program);
    return 0;
  }

  return program;
}

}  // namespace Impacto