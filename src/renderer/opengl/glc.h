#pragma once

#include "../../impacto.h"
#include "../renderer.h"

#include <array>

namespace Impacto {
namespace GLC {

inline std::array<GLuint, MaxFramebuffers> Framebuffers;
inline std::array<GLuint, MaxFramebuffers> FramebufferTextures;
inline std::array<GLuint, MaxFramebuffers> StencilBuffers;

void InitializeFramebuffers();

void BindFramebuffer(GLenum target, GLuint framebuffer);

void DeleteFramebuffers(GLsizei n, GLuint const* framebuffers);

}  // namespace GLC
}  // namespace Impacto