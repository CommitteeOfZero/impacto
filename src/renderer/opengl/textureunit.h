#pragma once

#include <glad/glad.h>
#include <memory>

namespace Impacto {
namespace OpenGL {

struct TextureUnitInfo {
  uint32_t TextureId = 0;
  bool Flushed = true;
};

class TextureUnit {
 public:
  TextureUnit(GLuint unit) : Unit(unit) {
    glGenSamplers(1, &Sampler);
    glBindSampler(unit, Sampler);

    glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glSamplerParameteri(Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(Sampler, GL_TEXTURE_MAX_ANISOTROPY, 16);
  }

  ~TextureUnit() {
    if (Info.use_count() == 1) glDeleteSamplers(1, &Sampler);
  }

  TextureUnit(const TextureUnit& other)
      : Unit(other.Unit), Sampler(other.Sampler), Info(other.Info) {
    Info->Flushed = false;
  }

  TextureUnit(TextureUnit&& other)
      : Unit(std::move(other.Unit)),
        Sampler(std::move(other.Sampler)),
        Info(std::move(other.Info)) {}

  TextureUnit& operator=(const TextureUnit& other) {
    if (Info.use_count() == 1 && &other != this) glDeleteSamplers(1, &Sampler);

    Unit = other.Unit;
    Sampler = other.Sampler;
    Info = other.Info;

    return *this;
  }

  TextureUnit& operator=(TextureUnit&& other) {
    if (Info.use_count() == 1 && &other != this) glDeleteSamplers(1, &Sampler);

    Unit = std::move(other.Unit);
    Sampler = std::move(other.Sampler);
    Info = std::move(other.Info);

    return *this;
  }

  GLuint GetUnit() const { return Unit; }
  operator GLuint() const { return GetUnit(); }

  bool IsLocked() const { return Info.use_count() > 1 || !Info->Flushed; }

  uint32_t GetTexture() const { return Info->TextureId; }
  bool SetTexture(uint32_t textureId) {
    if (IsLocked()) return false;

    Info->TextureId = textureId;
    glBindTextureUnit(Unit, textureId);

    return true;
  }

  GLuint GetSampler() const { return Sampler; }

  void Flush() {
    if (Info.use_count() == 1) Info->Flushed = true;
  }
  void Reserve() { Info->Flushed = false; }

 private:
  GLuint Unit;
  GLuint Sampler;
  std::shared_ptr<TextureUnitInfo> Info = std::make_shared<TextureUnitInfo>();
};

}  // namespace OpenGL
}  // namespace Impacto
