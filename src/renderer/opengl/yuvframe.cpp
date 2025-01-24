#include "yuvframe.h"
#include "../../log.h"

#include <glad/glad.h>

namespace Impacto {
namespace OpenGL {

void GLYUVFrame::Init(float width, float height) {
  Width = width;
  Height = height;
  GLuint yuv[3];

  glGenTextures(3, yuv);
  LumaId = yuv[0];
  CbId = yuv[1];
  CrId = yuv[2];
}

void GLYUVFrame::Submit(const void* luma, const void* cb, const void* cr) {
  glBindTexture(GL_TEXTURE_2D, LumaId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (GLsizei)Width, (GLsizei)Height, 0,
               GL_RED, GL_UNSIGNED_BYTE, luma);
  glBindTexture(GL_TEXTURE_2D, CbId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (GLsizei)(Width / 2),
               (GLsizei)(Height / 2), 0, GL_RED, GL_UNSIGNED_BYTE, cb);
  glBindTexture(GL_TEXTURE_2D, CrId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (GLsizei)(Width / 2),
               (GLsizei)(Height / 2), 0, GL_RED, GL_UNSIGNED_BYTE, cr);
}

void GLYUVFrame::Release() {
  GLuint yuv[3];
  yuv[0] = LumaId;
  yuv[1] = CbId;
  yuv[2] = CrId;
  glDeleteTextures(3, yuv);
}

}  // namespace OpenGL
}  // namespace Impacto