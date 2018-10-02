#pragma once

#include <glad/glad.h>

namespace Impacto {

void ShaderInit();
GLuint ShaderCompile(const char* name);

}  // namespace Impacto