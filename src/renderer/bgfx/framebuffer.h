#pragma once

#include "bgfxhandle.h"

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
  FrameBuffer(FrameBuffer&&) = default;
  ~FrameBuffer() = default;

  FrameBuffer& operator=(const FrameBuffer&) = delete;
  FrameBuffer& operator=(FrameBuffer&&) = default;

  FrameBuffer(uint16_t width, uint16_t height);

  [[nodiscard]] operator bgfx::FrameBufferHandle() {
    return GetFrameBufferHandle();
  }
  [[nodiscard]] bgfx::FrameBufferHandle GetFrameBufferHandle() {
    assert(FrameBufferHandle.IsValid());
    return FrameBufferHandle;
  }

  [[nodiscard]] bgfx::TextureHandle GetColorAttachmentHandle() {
    assert(ColorAttachment.IsValid());
    return ColorAttachment;
  }

  [[nodiscard]] bgfx::TextureHandle GetTexture() {
    assert(FrameBufferHandle.IsValid());
    return bgfx::getTexture(FrameBufferHandle, 0);
  }

  [[nodiscard]] glm::ivec2 GetSize() const { return Size; }

 private:
  BgfxHandle<bgfx::FrameBufferHandle> FrameBufferHandle;
  BgfxHandle<bgfx::TextureHandle> ColorAttachment;

  glm::ivec2 Size;
};

}  // namespace Impacto::Bgfx
