#include "framebuffer.h"

#include <array>

namespace Impacto::Bgfx {

FrameBuffer::FrameBuffer(const uint16_t width, const uint16_t height)
    : ColorAttachment(bgfx::createTexture2D(width, height, false, 1,
                                            bgfx::TextureFormat::RGBA8,
                                            BGFX_TEXTURE_RT)),
      StencilAttachment(bgfx::createTexture2D(width, height, false, 1,
                                              bgfx::TextureFormat::D24S8,
                                              BGFX_TEXTURE_RT)),
      Size(width, height) {
  assert(ColorAttachment.IsValid());
  assert(StencilAttachment.IsValid());

  std::array<bgfx::Attachment, 2> attachments;
  attachments[0].init(ColorAttachment);
  attachments[1].init(StencilAttachment);

  FrameBufferHandle = bgfx::createFrameBuffer(
      static_cast<uint8_t>(attachments.size()), attachments.data(), true);
  assert(FrameBufferHandle.IsValid());

  TextureObject = NonOwnedTexture(ColorAttachment, Size, true);
}

}  // namespace Impacto::Bgfx
