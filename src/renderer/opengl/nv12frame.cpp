#include "nv12frame.h"
#include "../../log.h"

#include <glad/glad.h>

namespace Impacto {
namespace OpenGL {

void GLNV12Frame::Init(float width, float height) {
  Width = width;
  Height = height;
  GLuint yuv[2];

  glGenTextures(2, yuv);
  LumaId = yuv[0];
  CbCrId = yuv[1];
}

void GLNV12Frame::Submit(const void* luma, const void* cbcr) {
  glBindTexture(GL_TEXTURE_2D, LumaId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (GLsizei)Width, (GLsizei)Height, 0,
               GL_RED, GL_UNSIGNED_BYTE, luma);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, CbCrId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, (GLsizei)(Width / 2),
               (GLsizei)(Height / 2), 0, GL_RG, GL_UNSIGNED_BYTE, cbcr);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void GLNV12Frame::Release() {
  GLuint yuv[2];
  yuv[0] = LumaId;
  yuv[1] = CbCrId;
  glDeleteTextures(2, yuv);
}

}  // namespace OpenGL
}  // namespace Impacto