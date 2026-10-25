#include "framebuffer.h"

#include <array>

namespace Impacto::Bgfx {

FrameBuffer::FrameBuffer(const uint16_t width, const uint16_t height)
    : ColorAttachment(bgfx::createTexture2D(width, height, false, 1,
                                            bgfx::TextureFormat::RGBA8,
                                            BGFX_TEXTURE_RT)),
      Size(width, height) {
  assert(ColorAttachment.IsValid());

  FrameBufferHandle = bgfx::createFrameBuffer(1, &ColorAttachment.Get(), true);
  assert(FrameBufferHandle.IsValid());
}

}  // namespace Impacto::Bgfx
