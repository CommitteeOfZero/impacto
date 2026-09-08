#pragma once

#include <bgfx/bgfx.h>

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

  [[nodiscard]] static FrameBuffer CreateBackBufferFrameBuffer();

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

 private:
  bgfx::FrameBufferHandle FrameBufferHandle = {bgfx::kInvalidHandle};
  bgfx::TextureHandle ColorAttachment = {bgfx::kInvalidHandle};
  bgfx::TextureHandle DepthStencilAttachment = {bgfx::kInvalidHandle};

  void Reset(bool cleanUpResources);
};

}  // namespace Impacto::Bgfx
