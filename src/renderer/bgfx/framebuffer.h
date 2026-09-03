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
    assert(IsInit && bgfx::isValid(FrameBufferHandle));
    return FrameBufferHandle;
  }

  [[nodiscard]] bgfx::TextureHandle GetColorAttachmentHandle() {
    assert(IsInit && bgfx::isValid(ColorAttachment));
    return ColorAttachment;
  }

  [[nodiscard]] bgfx::TextureHandle GetTexture() {
    assert(IsInit && bgfx::isValid(FrameBufferHandle));
    return bgfx::getTexture(FrameBufferHandle, 0);
  }

 private:
  bool IsInit = false;

  bgfx::FrameBufferHandle FrameBufferHandle;
  bgfx::TextureHandle ColorAttachment;
  std::optional<bgfx::TextureHandle> DepthStencilAttachment;

  void Reset(bool cleanUpResources);
};

}  // namespace Impacto::Bgfx
