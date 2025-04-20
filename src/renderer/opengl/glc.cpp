#include "glc.h"

#include "../../profile/game.h"

namespace Impacto {
namespace GLC {

static GLuint CurrentDrawFramebuffer = 0, CurrentReadFramebuffer = 0,
              CurrentFramebuffer = 0;

void InitializeFramebuffers() {
  glGenFramebuffers(Framebuffers.max_size(), Framebuffers.data());
  glGenTextures(FramebufferTextures.max_size(), FramebufferTextures.data());
  glGenRenderbuffers(StencilBuffers.max_size(), StencilBuffers.data());

  for (size_t buffer = 0; buffer < FramebufferTextures.size(); buffer++) {
    const GLuint framebufferId = Framebuffers[buffer];
    const GLuint textureId = FramebufferTextures[buffer];
    const GLuint stencilBufferId = StencilBuffers[buffer];

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window->WindowWidth,
                 Window->WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           textureId, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, stencilBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8,
                          Window->WindowWidth, Window->WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, stencilBufferId);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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
    default: {
      glBindFramebuffer(target, framebuffer);
    }
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