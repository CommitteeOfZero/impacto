#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include <cassert>
#include <span>
#include <optional>

namespace Impacto::Bgfx {

class FrameBuffer {
 public:
  FrameBuffer() = default;
  FrameBuffer(const FrameBuffer&) = delete;
  FrameBuffer(FrameBuffer&&);
  ~FrameBuffer();

  FrameBuffer& operator=(const FrameBuffer&) = delete;
  FrameBuffer& operator=(FrameBuffer&&);

  FrameBuffer(uint16_t width, uint16_t height);

  operator bgfx::FrameBufferHandle() { return GetFrameBufferHandle(); }

  [[nodiscard]] bgfx::FrameBufferHandle GetFrameBufferHandle() {
    assert(bgfx::isValid(FrameBufferHandle));
    return FrameBufferHandle;
  }

  [[nodiscard]] bgfx::TextureHandle GetColorAttachmentHandle() {
    assert(bgfx::isValid(ColorAttachment));
    return ColorAttachment;
  }

  [[nodiscard]] bgfx::TextureHandle GetTexture() {
    assert(bgfx::isValid(FrameBufferHandle));
    return bgfx::getTexture(FrameBufferHandle, 0);
  }

  [[nodiscard]] glm::ivec2 GetSize() const { return Size; }

 private:
  bgfx::FrameBufferHandle FrameBufferHandle = {bgfx::kInvalidHandle};
  bgfx::TextureHandle ColorAttachment = {bgfx::kInvalidHandle};

  glm::ivec2 Size;

  void Reset(bool cleanUpResources);
};

}  // namespace Impacto::Bgfx
