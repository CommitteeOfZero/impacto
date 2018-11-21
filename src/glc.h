#pragma once

#include "impacto.h"

namespace Impacto {
namespace GLC {

void BindFramebuffer(GLenum target, GLuint framebuffer);
void DeleteFramebuffers(GLsizei n, GLuint const* framebuffers);

}  // namespace GLC
}  // namespace Impacto