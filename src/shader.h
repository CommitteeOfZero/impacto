#pragma once

#include <GL/glew.h>

namespace Impacto {

void ShaderInit();
GLuint ShaderCompile(const char* name);

}  // namespace Impacto