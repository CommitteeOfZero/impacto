#include "glc.h"

namespace Impacto {
namespace GLC {

static GLuint CurrentDrawFramebuffer = 0, CurrentReadFramebuffer = 0,
              CurrentFramebuffer = 0;

void BindFramebuffer(GLenum target, GLuint framebuffer) {
  switch (target) {
    case GL_DRAW_FRAMEBUFFER: {
      if (CurrentDrawFramebuffer != framebuffer) {
        CurrentDrawFramebuffer = framebuffer;
        glBindFramebuffer(target, framebuffer);
      }
      break;
    }
    case GL_READ_FRAMEBUFFER: {
      if (CurrentReadFramebuffer != framebuffer) {
        CurrentReadFramebuffer = framebuffer;
        glBindFramebuffer(target, framebuffer);
      }
      break;
    }
    case GL_FRAMEBUFFER: {
      if (CurrentFramebuffer != framebuffer) {
        CurrentFramebuffer = framebuffer;
        glBindFramebuffer(target, framebuffer);
      }
      break;
    }
    default: { glBindFramebuffer(target, framebuffer); }
  }
}

void DeleteFramebuffers(GLsizei n, GLuint const* framebuffers) {
  glDeleteFramebuffers(n, framebuffers);
  for (GLsizei i = 0; i < n; i++) {
    if (CurrentDrawFramebuffer == framebuffers[i]) CurrentDrawFramebuffer = 0;
    if (CurrentReadFramebuffer == framebuffers[i]) CurrentReadFramebuffer = 0;
    if (CurrentFramebuffer == framebuffers[i]) CurrentFramebuffer = 0;
  }
}

}  // namespace GLC
}  // namespace Impacto